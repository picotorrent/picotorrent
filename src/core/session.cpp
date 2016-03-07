#include <picotorrent/core/session.hpp>

#include <picotorrent/core/environment.hpp>
#include <picotorrent/core/string_operations.hpp>
#include <picotorrent/core/version_info.hpp>
#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/configuration.hpp>
#include <picotorrent/core/session_metrics.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/core/torrent_info.hpp>
#include <picotorrent/core/filesystem/directory.hpp>
#include <picotorrent/core/filesystem/file.hpp>
#include <picotorrent/core/filesystem/path.hpp>
#include <picotorrent/core/logging/log.hpp>
#include <semver.hpp>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/peer_info.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <queue>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

#include <strsafe.h>

namespace fs = picotorrent::core::filesystem;
namespace lt = libtorrent;
using picotorrent::core::to_wstring;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;
using picotorrent::core::add_request;
using picotorrent::core::configuration;
using picotorrent::core::session;
using picotorrent::core::session_metrics;
using picotorrent::core::torrent;
using picotorrent::core::torrent_info;

struct load_item
{
    load_item(const fs::path &p)
        : path(p)
    {
    }

    fs::path path;
    std::vector<char> buffer;
    std::string magnet_uri;
    std::string save_path;
};

session::session()
    : hWnd_(NULL),
    metrics_(std::make_shared<session_metrics>(lt::session_stats_metrics()))
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
    p.save_path = "C:\\Downloads";

    // Add the info hash to our list of currently requested metadata files.
    loading_metadata_.insert({ p.info_hash, nullptr });

    sess_->async_add_torrent(p);
}

std::shared_ptr<session_metrics> session::metrics()
{
    return metrics_;
}

void session::load(HWND hWnd)
{
    LOG(info) << "Loading session";

    std::shared_ptr<lt::settings_pack> settings = get_session_settings();
    sess_ = std::make_unique<lt::session>(*settings);

    sess_->add_dht_router({ "router.bittorrent.com", 6881 });
    sess_->add_dht_router({ "router.utorrent.com", 6881 });
    sess_->add_dht_router({ "dht.transmissionbt.com", 6881 });
    sess_->add_dht_router({ "dht.aelitis.com", 6881 }); // Vuze

    load_state();
    load_torrents();

    hWnd_ = hWnd;
    sess_->set_alert_notify(std::bind(&session::on_alert_notify, this));
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

signal_connector<void, const session::torrent_ptr&>& session::on_torrent_updated()
{
    return on_torrent_updated_;
}

void session::on_alert_notify()
{
    PostMessage(hWnd_, WM_USER + 1337, NULL, NULL);
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

    fs::file torrent(it->second);

    if (!torrent.path().exists())
    {
        LOG(error) << "Torrent file did not exist when lazy loading.";
        ec.assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
    }
    else
    {
        torrent.read_all(buf);
    }
}

std::shared_ptr<lt::settings_pack> session::get_session_settings()
{
    std::shared_ptr<lt::settings_pack> settings = std::make_shared<lt::settings_pack>();

    configuration &cfg = configuration::instance();

    wchar_t iface[128];
    StringCchPrintf(iface, ARRAYSIZE(iface), L"%s:%d", cfg.listen_address().c_str(), cfg.listen_port());

    settings->set_bool(lt::settings_pack::enable_dht, true);
    settings->set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings->set_int(lt::settings_pack::alert_queue_size, cfg.alert_queue_size());
    settings->set_str(lt::settings_pack::listen_interfaces, to_string(iface));
    settings->set_int(lt::settings_pack::stop_tracker_timeout, cfg.stop_tracker_timeout());
    settings->set_int(lt::settings_pack::download_rate_limit, cfg.download_rate_limit());
    settings->set_int(lt::settings_pack::upload_rate_limit, cfg.upload_rate_limit());

    // Set PicoTorrent peer id and user agent
    if (cfg.use_picotorrent_peer_id())
    {
        std::wstring version = to_wstring(version_info::current_version());

        // Calculate user agent
        std::wstring user_agent = L"PicoTorrent/";
        user_agent.resize(user_agent.size() + version.size());
        StringCchPrintf(&user_agent[0], user_agent.size(), L"PicoTorrent/%s", version.c_str());

        // Calculate peer id
        semver::version v(version_info::current_version());
        std::wstring peer_id(L"-", 9);
        StringCchPrintf(&peer_id[0], peer_id.size(), L"-PI%d%02d%d-", v.getMajor(), v.getMinor(), v.getPatch());

        settings->set_str(lt::settings_pack::user_agent, to_string(user_agent));
        settings->set_str(lt::settings_pack::peer_fingerprint, to_string(peer_id));
    }
    else
    {
        settings->set_str(lt::settings_pack::user_agent, "libtorrent/" LIBTORRENT_VERSION);
        settings->set_str(lt::settings_pack::peer_fingerprint, "-LT1100-");
    }

    // Proxy settings
    settings->set_int(lt::settings_pack::proxy_type, cfg.proxy_type());
    settings->set_str(lt::settings_pack::proxy_hostname, to_string(cfg.proxy_host()));
    settings->set_int(lt::settings_pack::proxy_port, cfg.proxy_port());
    settings->set_bool(lt::settings_pack::force_proxy, cfg.proxy_force());
    settings->set_bool(lt::settings_pack::proxy_hostnames, cfg.proxy_hostnames());
    settings->set_bool(lt::settings_pack::proxy_peer_connections, cfg.proxy_peers());
    settings->set_bool(lt::settings_pack::proxy_tracker_connections, cfg.proxy_trackers());

    return settings;

}

void session::load_state()
{
    fs::directory data = environment::get_data_path();
    fs::file state = data.path().combine(L"Session.dat");
    std::vector<char> buffer;

    if (!state.path().exists())
    {
        LOG(trace) << "State file does not exist";
        return;
    }

    try
    {
        state.read_all(buffer);
    }
    catch (const std::exception& e)
    {
        LOG(error) << "Error when reading file: " << e.what();
        return;
    }

    lt::bdecode_node node;
    lt::error_code ec;
    lt::bdecode(&buffer[0], &buffer[0] + buffer.size(), node, ec);

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
    fs::path data = environment::get_data_path();
    fs::directory torrents = data.combine(L"Torrents");

    if (!torrents.path().exists())
    {
        LOG(trace) << "Torrents directory does not exist";
        return;
    }

    std::vector<fs::path> files = torrents.get_files(torrents.path().combine(L"*.dat"));

    LOG(info) << "Loading " << files.size() << " torrent(s)";

    typedef std::pair<int64_t, load_item> prio_item_t;
    auto comparer = [](const prio_item_t &lhs, const prio_item_t &rhs)
    {
        return lhs.first > rhs.first;
    };

    std::priority_queue<prio_item_t, std::vector<prio_item_t>, decltype(comparer)> queue(comparer);
    int64_t maxPosition = std::numeric_limits<int64_t>::max();

    for (fs::path &path : files)
    {
        load_item item(path);
        fs::file resumeFile(path);

        try
        {
            resumeFile.read_all(item.buffer);
        }
        catch (const std::exception& e)
        {
            LOG(error) << "Error when reading file: " << e.what();
            continue;
        }

        lt::error_code ec;
        lt::bdecode_node node;
        lt::bdecode(&item.buffer[0], &item.buffer[0] + item.buffer.size(), node, ec);

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
        item.save_path = node.dict_find_string_value("pT-savePath", "C:\\Downloads");

        int64_t queuePosition = node.dict_find_int_value("pT-queuePosition", maxPosition);
        if (queuePosition < 0) { queuePosition = maxPosition; }

        queue.push({ queuePosition, item });
    }

    while (!queue.empty())
    {
        load_item item = queue.top().second;
        queue.pop();

        fs::path torrentPath = item.path.replace_extension(L".torrent");

        if (!torrentPath.exists() && item.magnet_uri.empty())
        {
            LOG(error) << "Torrent does not exist (although resume file does)";
            fs::file(item.path).remove();
            continue;
        }

        lt::add_torrent_params params;

        if (torrentPath.exists())
        {
            fs::file torrent(torrentPath);
            std::vector<char> buffer;

            try
            {
                torrent.read_all(buffer);
            }
            catch (const std::exception &ex)
            {
                LOG(error) << "Error when reading file: " << ex.what();
                continue;
            }

            lt::bdecode_node node;
            lt::error_code ec;
            lt::bdecode(&buffer[0], &buffer[0] + buffer.size(), node, ec);

            if (ec)
            {
                LOG(error) << "Error when bdecoding buffer: " << ec.message();
                continue;
            }

            params.ti = boost::make_shared<lt::torrent_info>(node);

            // Insert into hash-to-path map
            hash_to_path_.insert({ params.ti->info_hash(), torrent.path().to_string() });
        }

        params.flags |= lt::add_torrent_params::flags_t::flag_use_resume_save_path;
        params.save_path = item.save_path;
        params.url = item.magnet_uri;

        if(!item.buffer.empty())
        {
            params.resume_data = item.buffer;
        }

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

                fs::path data = environment::get_data_path();
                fs::directory dir = data.combine(L"Torrents");

                if (!dir.path().exists())
                {
                    dir.create();
                }

                std::wstring hash = lt::convert_to_wstring(lt::to_hex(al->handle.info_hash().to_string()));
                fs::file torrentFile(dir.path().combine((hash + L".dat")));

                try
                {
                    torrentFile.write_all(buf);
                }
                catch (const std::exception& e)
                {
                    LOG(error) << "Error when writing resume data file: " << e.what();
                }
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

                fs::directory torrents = environment::get_data_path().combine(L"Torrents");
                std::wstring hash = lt::convert_to_wstring(lt::to_hex(al->handle.info_hash().to_string()));
                fs::path torrent(torrents.path().combine((hash + L".torrent")));

                hash_to_path_.insert({ al->handle.info_hash(), torrent.to_string() });
            }
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

            for (lt::torrent_status &st : al->status)
            {
                if (loading_metadata_.find(st.info_hash) != loading_metadata_.end())
                {
                    continue;
                }

                const torrent_ptr &t = torrents_.find(st.info_hash)->second;
                t->update(std::make_unique<lt::torrent_status>(st));
                on_torrent_updated_.emit(t);
            }
            break;
        }
        case lt::torrent_finished_alert::alert_type:
        {
            lt::torrent_finished_alert *al = lt::alert_cast<lt::torrent_finished_alert>(alert);
            torrent_map_t::iterator &find = torrents_.find(al->handle.info_hash());

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
            torrent_map_t::iterator &find = torrents_.find(al->handle.info_hash());
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

void session::reload_settings()
{
    sess_->apply_settings(*get_session_settings());
}

void session::remove_torrent_files(const torrent_ptr &torrent)
{
    std::wstring hash = lt::convert_to_wstring(lt::to_hex(torrent->status_->info_hash.to_string()));

    fs::path data = environment::get_data_path();
    fs::file file = data.combine(L"Torrents").combine((hash + L".torrent"));

    if (file.path().exists())
    {
        file.remove();
    }

    file = file.path().replace_extension(L".dat");

    if (file.path().exists())
    {
        file.remove();
    }
}

void session::save_state()
{
    // Save session state to "Session.dat" in the
    // current directory.
    lt::entry e;
    sess_->save_state(e);

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), e);

    fs::directory data = environment::get_data_path();

    if (!data.path().exists())
    {
        data.create();
    }

    fs::file state = data.path().combine(L"Session.dat");

    try
    {
        state.write_all(buf);
    }
    catch (const std::exception& e)
    {
        LOG(error) << "Error when writing buffer: " << e.what();
    }

    LOG(info) << "Session state saved";
}

void session::save_torrent(const lt::torrent_info &ti)
{
    fs::path data = environment::get_data_path();
    fs::directory dir = data.combine(L"Torrents");

    if (!dir.path().exists())
    {
        dir.create();
    }

    lt::create_torrent ct(ti);
    lt::entry e = ct.generate();

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), e);

    std::wstring hash = lt::convert_to_wstring(lt::to_hex(ti.info_hash().to_string()));
    fs::file torrentFile(dir.path().combine((hash + L".torrent")));

    try
    {
        torrentFile.write_all(buf);
    }
    catch (const std::exception &e)
    {
        LOG(error) << "Error when writing torrent file: " << e.what();
    }
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

    fs::path data = environment::get_data_path();
    fs::directory dir = data.combine(L"Torrents");

    if (!dir.path().exists())
    {
        dir.create();
    }

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

            // Insert PicoTorrent-specific state
            rd->resume_data->dict().insert({ "pT-queuePosition", rd->handle.status().queue_position });

            std::vector<char> buffer;
            lt::bencode(std::back_inserter(buffer), *rd->resume_data);

            std::wstring hash = lt::convert_to_wstring(lt::to_hex(rd->handle.info_hash().to_string()));
            fs::file torrentFile(dir.path().combine((hash + L".dat")));

            try
            {
                torrentFile.write_all(buffer);
            }
            catch (const std::exception& e)
            {
                LOG(error) << "Error when writing resume data file: " << e.what();
            }
        }
    }
}
