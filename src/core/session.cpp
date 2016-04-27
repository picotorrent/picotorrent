#include <picotorrent/core/session.hpp>

#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/hash.hpp>
#include <picotorrent/core/pal.hpp>
#include <picotorrent/core/session_configuration.hpp>
#include <picotorrent/core/session_metrics.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/core/torrent_info.hpp>
#include <picotorrent/core/version_info.hpp>
#include <semver.hpp>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <fstream>
#include <iomanip>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/peer_info.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <queue>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

#define LOG(level) \
    session_log_item(*config_->session_log_stream).stream()

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#endif

namespace lt = libtorrent;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;
using picotorrent::core::add_request;
using picotorrent::core::hash;
using picotorrent::core::pal;
using picotorrent::core::session;
using picotorrent::core::session_configuration;
using picotorrent::core::session_metrics;
using picotorrent::core::torrent;
using picotorrent::core::torrent_info;

struct load_item
{
    load_item(const std::string &p)
        : path(p)
    {
    }

    std::string path;
    std::string resume_data;
    std::string magnet_uri;
    std::string save_path;
};

struct session_log_item
{
    session_log_item(std::ostream &stream)
        : stream_(stream)
    {
    }

    ~session_log_item()
    {
        stream_ << std::endl;
    }

    std::ostream& stream()
    {
        return stream_;
    }

private:
    std::ostream& stream_;
};

session::session(const std::shared_ptr<session_configuration> &config)
    : metrics_(std::make_shared<session_metrics>(lt::session_stats_metrics())),
    config_(config)
{
}

session::~session()
{
}

void session::add_torrent(const std::shared_ptr<add_request> &add)
{
    sess_->async_add_torrent(*add->params_);
}

void session::get_metadata(const std::string &magnet)
{
    lt::add_torrent_params p;
    lt::error_code ec;
    lt::parse_magnet_uri(magnet, p, ec);

    if (ec)
    {
        LOG(error) << "Could not parse magnet link: " << ec.message();
        return;
    }

    if (loading_metadata_.find(p.info_hash) != loading_metadata_.end())
    {
        LOG(warning) << "Torrent already exists in session: " << p.info_hash;
        return;
    }

    // Forced start
    p.flags &= ~lt::add_torrent_params::flag_paused;
    p.flags &= ~lt::add_torrent_params::flag_auto_managed;
    p.flags |=  lt::add_torrent_params::flag_upload_mode;

    // Set a temporary save path
    std::string ih = lt::to_hex(p.info_hash.to_string());
    p.save_path = pal::combine_paths(config_->temporary_directory, ih);

    // Add the info hash to our list of currently requested metadata files.
    loading_metadata_.insert({ p.info_hash, nullptr });
    sess_->async_add_torrent(p);
}

bool session::has_torrent(const std::shared_ptr<hash> &hash)
{
    return (torrents_.find(*hash) != torrents_.end());
}

std::shared_ptr<session_metrics> session::metrics()
{
    return metrics_;
}

void session::load()
{
    LOG(info) << "Loading session";

    std::shared_ptr<lt::settings_pack> settings = get_session_settings();
    sess_ = std::unique_ptr<lt::session>(new lt::session(*settings));

    sess_->add_dht_router({ "router.bittorrent.com", 6881 });
    sess_->add_dht_router({ "router.utorrent.com", 6881 });
    sess_->add_dht_router({ "dht.transmissionbt.com", 6881 });
    sess_->add_dht_router({ "dht.aelitis.com", 6881 }); // Vuze

    load_state();
    load_torrents();

    sess_->set_alert_notify([this] {
        on_notifications_available_.emit();
    });

    sess_->set_load_function(
        std::bind(
            &session::on_load_torrent,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));
}

void session::unload()
{
    LOG(info) << "Unloading session";
    sess_->set_alert_notify([] {});

    save_state();
    save_torrents();
}

void session::remove_torrent(const torrent_ptr &torrent, bool remove_data)
{
    sess_->remove_torrent(torrent->status_->handle, remove_data ? lt::session::delete_files : 0);
}

signal_connector<void, const std::shared_ptr<torrent_info>&>& session::on_metadata_received()
{
    return on_metadata_received_;
}

signal_connector<void, void>& session::on_notifications_available()
{
    return on_notifications_available_;
}

signal_connector<void, const session::torrent_ptr&>& session::on_torrent_added()
{
    return on_torrent_added_;
}

signal_connector<void, const session::torrent_ptr&>& session::on_torrent_finished()
{
    return on_torrent_finished_;
}

signal_connector<void, const session::torrent_ptr&>& session::on_torrent_removed()
{
    return on_torrent_removed_;
}

signal_connector<void, const std::vector<session::torrent_ptr>&>& session::on_torrent_updated()
{
    return on_torrent_updated_;
}

void session::on_load_torrent(const lt::sha1_hash &hash, std::vector<char> &buf, lt::error_code &ec)
{
    auto it = hash_to_path_.find(hash);

    if (it == hash_to_path_.end())
    {
        LOG(error) << "Hash-to-path map did not contain the info hash.";
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
        return;
    }

    std::ifstream input(it->second, std::ios::binary);

    if (!input)
    {
        LOG(error) << "Torrent file did not exist when lazy loading.";
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
    }

    input.seekg(0, std::ios::end);
    std::streampos size = input.tellg();
    input.seekg(0, std::ios::beg);

    buf.reserve((size_t)size);
    buf.insert(
        buf.begin(),
        std::istream_iterator<char>(input),
        std::istream_iterator<char>());
}

std::shared_ptr<lt::settings_pack> session::get_session_settings()
{
    std::shared_ptr<lt::settings_pack> settings = std::make_shared<lt::settings_pack>();

    std::stringstream ifaces;
    for (auto &p : config_->listen_interfaces)
    {
        ifaces << "," << p.first << ":" << p.second;
    }

    settings->set_bool(lt::settings_pack::enable_dht, config_->enable_dht);
    settings->set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings->set_int(lt::settings_pack::alert_queue_size, config_->alert_queue_size);
    settings->set_str(lt::settings_pack::listen_interfaces, ifaces.str().substr(1));
    settings->set_int(lt::settings_pack::stop_tracker_timeout, config_->stop_tracker_timeout);
    settings->set_int(lt::settings_pack::download_rate_limit, config_->download_rate_limit);
    settings->set_int(lt::settings_pack::upload_rate_limit, config_->upload_rate_limit);

    // Limits
    settings->set_int(lt::settings_pack::active_checking, config_->active_checking);
    settings->set_int(lt::settings_pack::active_dht_limit, config_->active_dht_limit);
    settings->set_int(lt::settings_pack::active_downloads, config_->active_downloads);
    settings->set_int(lt::settings_pack::active_limit, config_->active_limit);
    settings->set_int(lt::settings_pack::active_loaded_limit, config_->active_loaded_limit);
    settings->set_int(lt::settings_pack::active_lsd_limit, config_->active_lsd_limit);
    settings->set_int(lt::settings_pack::active_seeds, config_->active_seeds);
    settings->set_int(lt::settings_pack::active_tracker_limit, config_->active_tracker_limit);

    // Calculate user agent
    std::stringstream user_agent;
    user_agent << "PicoTorrent/" << version_info::current_version();

    // Calculate peer id
    semver::version v(version_info::current_version());
    std::stringstream peer_id;
    peer_id << "-PI" << v.getMajor() << std::setfill('0') << std::setw(2) << v.getMinor() << v.getPatch() << "-";

    settings->set_str(lt::settings_pack::user_agent, user_agent.str());
    settings->set_str(lt::settings_pack::peer_fingerprint, peer_id.str());

    // Proxy settings
    settings->set_int(lt::settings_pack::proxy_type, config_->proxy_type);
    settings->set_str(lt::settings_pack::proxy_hostname, config_->proxy_host);
    settings->set_int(lt::settings_pack::proxy_port, config_->proxy_port);
    settings->set_bool(lt::settings_pack::force_proxy, config_->proxy_force);
    settings->set_bool(lt::settings_pack::proxy_hostnames, config_->proxy_hostnames);
    settings->set_bool(lt::settings_pack::proxy_peer_connections, config_->proxy_peers);
    settings->set_bool(lt::settings_pack::proxy_tracker_connections, config_->proxy_trackers);

    return settings;

}

void session::load_state()
{
    std::ifstream sf(config_->session_state_file, std::ifstream::in | std::ifstream::binary);
    if (!sf) { return; }

    std::stringstream buf;
    buf << sf.rdbuf();
    std::string contents = buf.str();

    lt::bdecode_node node;
    lt::error_code ec;
    lt::bdecode(&contents[0], &contents[0] + contents.size(), node, ec);

    if (ec)
    {
        LOG(error) << "Error when bdecoding buffer: " << ec.message();
        return;
    }

    sess_->load_state(node);
    LOG(info) << "Session state loaded";
}

void session::load_torrents()
{
    if (!pal::directory_exists(config_->torrents_directory))
    {
        LOG(trace) << "Torrents directory does not exist";
        return;
    }

    std::vector<std::string> torrents = pal::get_directory_entries(config_->torrents_directory, "*.dat");
    LOG(info) << "Loading " << torrents.size() << " torrent(s)";

    typedef std::pair<int64_t, load_item> prio_item_t;
    auto comparer = [](const prio_item_t &lhs, const prio_item_t &rhs)
    {
        return lhs.first > rhs.first;
    };

    std::priority_queue<prio_item_t, std::vector<prio_item_t>, decltype(comparer)> queue(comparer);
    int64_t maxPosition = std::numeric_limits<int64_t>::max();

    for (std::string &path : torrents)
    {
        load_item item(path);

        std::ifstream sf(path, std::ios::binary);
        if (!sf) { continue; }

        std::stringstream buf;
        buf << sf.rdbuf();
        item.resume_data = buf.str();

        lt::error_code ec;
        lt::bdecode_node node;
        lt::bdecode(
            &item.resume_data[0],
            &item.resume_data[0] + item.resume_data.size(),
            node,
            ec);

        if (ec)
        {
            LOG(error) << "Error when bdecoding buffer: " << ec.message();
            continue;
        }

        if (node.type() != lt::bdecode_node::type_t::dict_t)
        {
            LOG(error) << "Resume file not a bencoded dictionary (" << node.type() << ")";
            continue;
        }

        item.magnet_uri = node.dict_find_string_value("pT-url");
        item.save_path = node.dict_find_string_value("pT-savePath", config_->default_save_path.c_str());

        int64_t queuePosition = node.dict_find_int_value("pT-queuePosition", maxPosition);
        if (queuePosition < 0) { queuePosition = maxPosition; }

        queue.push({ queuePosition, item });
    }

    while (!queue.empty())
    {
        load_item item = queue.top().second;
        queue.pop();

        std::string torrent_file = pal::replace_extension(item.path, ".torrent");

        if (!pal::file_exists(torrent_file) && item.magnet_uri.empty())
        {
            LOG(error) << "Torrent does not exist (although resume file does)";
            pal::remove_file(item.path);
            continue;
        }

        lt::add_torrent_params params;

        if (!item.resume_data.empty())
        {
            lt::error_code ec;
            params = lt::read_resume_data(
                &item.resume_data[0],
                (int)item.resume_data.size(),
                ec);

            if (ec)
            {
                LOG(error) << "Failed to read resume data, " << ec.message();
                continue;
            }
        }

        if (pal::file_exists(torrent_file))
        {
            std::ifstream sf(torrent_file, std::ifstream::in | std::ifstream::binary);

            if (!sf)
            {
                LOG(error) << "Error when reading file: " << torrent_file;
                continue;
            }

            std::stringstream buf;
            buf << sf.rdbuf();
            std::string contents = buf.str();

            lt::bdecode_node node;
            lt::error_code ec;
            lt::bdecode(&contents[0], &contents[0] + contents.size(), node, ec);

            if (ec)
            {
                LOG(error) << "Error when bdecoding buffer: " << ec.message();
                continue;
            }

            params.ti = boost::make_shared<lt::torrent_info>(node);

            // Insert into hash-to-path map
            hash_to_path_.insert({ params.ti->info_hash(), torrent_file });
        }

        if (params.save_path.empty())
        {
            params.save_path = item.save_path;
        }

        params.url = item.magnet_uri;

        sess_->async_add_torrent(params);
    }
}

void session::notify()
{
    std::vector<lt::alert*> alerts;
    sess_->pop_alerts(&alerts);

    for (lt::alert *alert : alerts)
    {
        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            lt::add_torrent_alert *al = lt::alert_cast<lt::add_torrent_alert>(alert);

            if (al->error)
            {
                LOG(error) << "Error when adding torrent: " << al->error.message();
                continue;
            }

            if (loading_metadata_.find(al->handle.info_hash()) != loading_metadata_.end())
            {
                // Ignore the torrent if we only search for its metadata
                continue;
            }

            if (torrents_.find(al->handle.info_hash()) != torrents_.end())
            {
                LOG(warning) << "Torrent already in session: " << lt::to_hex(al->handle.info_hash().to_string());
                continue;
            }

            if (al->handle.torrent_file())
            {
                save_torrent(*al->handle.torrent_file());
            }
            else if (!al->params.url.empty())
            {
                lt::entry::dictionary_type e;
                e.insert({ "pT-queuePosition", al->handle.status().queue_position });
                e.insert({ "pT-url", al->params.url });

                std::vector<char> buf;
                lt::bencode(std::back_inserter(buf), e);

                std::string hash = lt::to_hex(al->handle.info_hash().to_string());
                std::string torrent_dat = pal::combine_paths(config_->torrents_directory, (hash + ".dat"));

                if (!pal::directory_exists(config_->torrents_directory))
                {
                    pal::create_directories(config_->torrents_directory);
                }

                std::ofstream tos(torrent_dat, std::ios::binary);
                tos.write(&buf[0], buf.size());
                tos.close();
            }

            torrent_ptr torrent = std::make_shared<core::torrent>(al->handle.status());
            torrents_.insert({ al->handle.info_hash(), torrent });
            on_torrent_added_.emit(torrent);
            break;
        }
        case lt::metadata_received_alert::alert_type:
        {
            lt::metadata_received_alert *al = lt::alert_cast<lt::metadata_received_alert>(alert);

            if (loading_metadata_.find(al->handle.info_hash()) != loading_metadata_.end())
            {
                // We have the metadata. Load it into a shared pointer and emit an event.
                std::shared_ptr<torrent_info> ti = std::make_shared<torrent_info>(*al->handle.torrent_file());
                sess_->remove_torrent(al->handle);
                on_metadata_received_.emit(ti);
                continue;
            }

            if (al->handle.torrent_file())
            {
                save_torrent(*al->handle.torrent_file());

                std::string hash = lt::to_hex(al->handle.info_hash().to_string());
                std::string torrent_file = pal::combine_paths(config_->torrents_directory, (hash + ".torrent"));
                hash_to_path_.insert({ al->handle.info_hash(), torrent_file });
            }
            break;
        }
        case lt::save_resume_data_alert::alert_type:
        {
            lt::save_resume_data_alert *al = lt::alert_cast<lt::save_resume_data_alert>(alert);
            if (!al->resume_data) { continue; }
            save_resume_data(al->handle, *al->resume_data);
            break;
        }
        case lt::session_stats_alert::alert_type:
        {
            lt::session_stats_alert *al = lt::alert_cast<lt::session_stats_alert>(alert);
            metrics_->update(al->values, ARRAYSIZE(al->values));
            break;
        }
        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert *al = lt::alert_cast<lt::state_update_alert>(alert);
            std::vector<std::shared_ptr<torrent>> updated;

            for (lt::torrent_status &st : al->status)
            {
                if (loading_metadata_.find(st.info_hash) != loading_metadata_.end())
                {
                    continue;
                }

                const torrent_ptr &t = torrents_.find(st.info_hash)->second;
                t->update(std::unique_ptr<lt::torrent_status>(new lt::torrent_status(st)));
                updated.push_back(t);
            }

            on_torrent_updated_.emit(updated);
            break;
        }
        case lt::torrent_added_alert::alert_type:
        {
            lt::torrent_added_alert *al = lt::alert_cast<lt::torrent_added_alert>(alert);
            if (al->handle.need_save_resume_data()) { al->handle.save_resume_data(); }
            break;
        }
        case lt::torrent_finished_alert::alert_type:
        {
            lt::torrent_finished_alert *al = lt::alert_cast<lt::torrent_finished_alert>(alert);
            if (al->handle.need_save_resume_data()) { al->handle.save_resume_data(); }

            auto find = torrents_.find(al->handle.info_hash());

            // Check `total_download` to see if we have a real finished torrent or one that
            // was finished when we added it and just completed the hash check.
            if (find != torrents_.end() && al->handle.status().total_download > 0)
            {
                on_torrent_finished_.emit(find->second);
            }
            break;
        }
        case lt::torrent_removed_alert::alert_type:
        {
            lt::torrent_removed_alert *al = lt::alert_cast<lt::torrent_removed_alert>(alert);

            if (loading_metadata_.find(al->info_hash) != loading_metadata_.end())
            {
                loading_metadata_.erase(al->info_hash);
                continue;
            }

            torrent_ptr &torrent = torrents_.at(al->info_hash);

            remove_torrent_files(torrent);
            on_torrent_removed_.emit(torrent);
            torrents_.erase(al->info_hash);
            break;
        }
        case lt::tracker_reply_alert::alert_type:
        {
            lt::tracker_reply_alert *al = lt::alert_cast<lt::tracker_reply_alert>(alert);
            auto find = torrents_.find(al->handle.info_hash());
            if (find != torrents_.end()) { find->second->handle(*al); }
            break;
        }
        case lt::scrape_reply_alert::alert_type:
            lt::scrape_reply_alert *al = lt::alert_cast<lt::scrape_reply_alert>(alert);
            torrent_ptr &torrent = torrents_.at(al->handle.info_hash());
            torrent->handle(*al);
            break;
        }
    }
}

void session::post_updates()
{
    sess_->post_dht_stats();
    sess_->post_session_stats();
    sess_->post_torrent_updates();
}

void session::reload_settings(const std::shared_ptr<session_configuration> &config)
{
    config_ = config;
    sess_->apply_settings(*get_session_settings());
}

void session::remove_torrent_files(const torrent_ptr &torrent)
{
    std::string hash = lt::to_hex(torrent->status_->info_hash.to_string());
    std::string torrent_file = pal::combine_paths(config_->torrents_directory, (hash + ".torrent"));
    std::string torrent_dat = pal::combine_paths(config_->torrents_directory, (hash + ".dat"));

    if (pal::file_exists(torrent_file)) { pal::remove_file(torrent_file); }
    if (pal::file_exists(torrent_dat)) { pal::remove_file(torrent_dat); }
}

void session::save_resume_data(const lt::torrent_handle &th, lt::entry &entry)
{
    // Insert PicoTorrent-specific state
    entry.dict().insert({ "pT-queuePosition", th.status().queue_position });

    std::vector<char> buffer;
    lt::bencode(std::back_inserter(buffer), entry);

    std::string hash = lt::to_hex(th.info_hash().to_string());
    std::string torrent_dat = pal::combine_paths(config_->torrents_directory, (hash + ".dat"));

    if (!pal::directory_exists(config_->torrents_directory))
    {
        pal::create_directories(config_->torrents_directory);
    }
    
    std::ofstream tos(torrent_dat, std::ios::binary);
    tos.write(&buffer[0], buffer.size());
}

void session::save_state()
{
    lt::entry e;
    sess_->save_state(e);

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), e);

    std::ofstream tos(config_->session_state_file, std::ios::binary);
    tos.write(&buf[0], buf.size());

    LOG(info) << "Session state saved";
}

void session::save_torrent(const lt::torrent_info &ti)
{
    lt::create_torrent ct(ti);
    lt::entry e = ct.generate();

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), e);

    std::string hash = lt::to_hex(ti.info_hash().to_string());
    std::string torrent_file = pal::combine_paths(config_->torrents_directory, (hash + ".torrent"));

    if (!pal::directory_exists(config_->torrents_directory))
    {
        pal::create_directories(config_->torrents_directory);
    }

    std::ofstream tos(torrent_file, std::ios::binary);
    tos.write(&buf[0], buf.size());
}

void session::save_torrents()
{
    sess_->pause();

    // Save each torrents resume data
    int numOutstandingResumeData = 0;
    int numPaused = 0;
    int numFailed = 0;

    std::vector<lt::torrent_status> temp;
    sess_->get_torrent_status(&temp, [](const lt::torrent_status &st) { return true; }, 0);

    for (lt::torrent_status &st : temp)
    {
        if (!st.handle.is_valid())
        {
            // TODO(log)
            continue;
        }

        if (!st.has_metadata)
        {
            // TODO(log)
            continue;
        }

        if (!st.need_save_resume)
        {
            // TODO(log)
            continue;
        }

        st.handle.save_resume_data();
        ++numOutstandingResumeData;
    }

    LOG(info) << "Saving resume data for " << numOutstandingResumeData << " torrent(s)";

    while (numOutstandingResumeData > 0)
    {
        const lt::alert *a = sess_->wait_for_alert(lt::seconds(10));
        if (a == 0) { continue; }

        std::vector<lt::alert*> alerts;
        sess_->pop_alerts(&alerts);

        for (lt::alert *a : alerts)
        {
            lt::torrent_paused_alert *tp = lt::alert_cast<lt::torrent_paused_alert>(a);

            if (tp)
            {
                ++numPaused;
                // TODO(log)
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++numFailed;
                --numOutstandingResumeData;
                // TODO(log)
                continue;
            }

            lt::save_resume_data_alert *rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --numOutstandingResumeData;
            if (!rd->resume_data) { continue; }

            save_resume_data(rd->handle, *rd->resume_data);
        }
    }
}
