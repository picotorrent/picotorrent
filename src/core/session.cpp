#include <picotorrent/core/session.hpp>

#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

#include <picotorrent/common/environment.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/config/configuration.hpp>
#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/timer.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/filesystem/directory.hpp>
#include <picotorrent/filesystem/file.hpp>
#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/logging/log.hpp>

#include <queue>

namespace fs = picotorrent::filesystem;
namespace lt = libtorrent;
using namespace picotorrent::common;
using picotorrent::config::configuration;
using picotorrent::core::add_request;
using picotorrent::core::session;
using picotorrent::core::timer;
using picotorrent::core::torrent;

struct load_item
{
    load_item(const fs::path &p)
        : path(p)
    {
    }

    fs::path path;
    lt::bdecode_node node;
    std::vector<char> buffer;
};

session::session()
    : timer_(std::make_unique<timer>(std::bind(&session::timer_callback, this), 1000))
{
}

session::~session()
{
}

void session::add_torrent(const add_request &add)
{
    sess_->async_add_torrent(*add.params_);
}

void session::load()
{
    LOG(info) << "Loading session";

    configuration &cfg = configuration::instance();
    std::wstring iface(cfg.listen_interface() + L":" + std::to_wstring(cfg.listen_port()));

    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings.set_int(lt::settings_pack::alert_queue_size, cfg.alert_queue_size());
    settings.set_str(lt::settings_pack::listen_interfaces, to_string(iface));
    settings.set_int(lt::settings_pack::stop_tracker_timeout, cfg.stop_tracker_timeout());

    sess_ = std::make_unique<lt::session>(settings);

    load_state();
    load_torrents();

    // Start the alert thread which will read and handle any
    // alerts we receive from libtorrent.
    is_running_ = true;
    alert_thread_ = std::thread(std::bind(&session::read_alerts, this));

    timer_->start();
}

void session::unload()
{
    LOG(info) << "Unloading session";
    is_running_ = false;

    timer_->stop();

    if (alert_thread_.joinable())
    {
        LOG(trace) << "Joining alert thread";
        alert_thread_.join();
    }

    save_state();
    save_torrents();
}

void session::on_torrent_added(const std::function<void(const torrent_ptr&)> &callback)
{
    torrent_added_cb_ = callback;
}

void session::on_torrent_removed(const std::function<void()> &callback)
{
}

void session::on_torrent_updated(const std::function<void(const torrent_ptr&)> &callback)
{
    torrent_updated_cb_ = callback;
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
        lt::bdecode(&item.buffer[0], &item.buffer[0] + item.buffer.size(), item.node, ec);

        if (ec)
        {
            LOG(error) << "Error when bdecoding buffer: " << ec.message();
            continue;
        }

        if (item.node.type() != lt::bdecode_node::type_t::dict_t)
        {
            LOG(error) << "Resume file not a bencoded dictionary (" << item.node.type() << ")";
            continue;
        }

        int64_t queuePosition = item.node.dict_find_int_value("pT-queuePosition", maxPosition);
        if (queuePosition < 0) { queuePosition = maxPosition; }

        queue.push({ queuePosition, item });
    }

    while (!queue.empty())
    {
        load_item item = queue.top().second;
        queue.pop();

        fs::path torrentPath = item.path.replace_extension(L".torrent");

        if (!torrentPath.exists())
        {
            LOG(error) << "Torrent does not exist (although resume file does)";
            // TODO(remove resume file)
            continue;
        }

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

        lt::add_torrent_params params;
        params.flags |= lt::add_torrent_params::flags_t::flag_use_resume_save_path;
        params.save_path = "C:\\Downloads";
        params.ti = boost::make_shared<lt::torrent_info>(node);

        if(!item.buffer.empty())
        {
            params.resume_data = item.buffer;
        }

        LOG(info) << "Adding torrent " << params.ti->name();
        sess_->async_add_torrent(params);
    }
}

void session::read_alerts()
{
    while (is_running_)
    {
        const lt::alert *a = sess_->wait_for_alert(lt::milliseconds(500));
        if (a == 0) { continue; }

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

                if (torrents_.find(al->handle.info_hash()) != torrents_.end())
                {
                    LOG(warning) << "Torrent already in session: " << lt::to_hex(al->handle.info_hash().to_string());
                    continue;
                }

                if (al->handle.torrent_file())
                {
                    save_torrent(*al->handle.torrent_file());
                }

                torrents_.insert(
                    std::make_pair(
                        al->handle.info_hash(),
                        std::make_shared<core::torrent>(al->handle.status())
                        ));

                if (torrent_added_cb_)
                {
                    const torrent_ptr &t = torrents_.find(al->handle.info_hash())->second;
                    torrent_added_cb_(t);
                }
                break;
            }
            case lt::state_update_alert::alert_type:
            {
                lt::state_update_alert *al = lt::alert_cast<lt::state_update_alert>(alert);

                for (lt::torrent_status &st : al->status)
                {
                    const torrent_ptr &t = torrents_.find(st.info_hash)->second;
                    t->status_ = std::make_unique<lt::torrent_status>(st);

                    if (torrent_updated_cb_)
                    {
                        torrent_updated_cb_(t);
                    }
                }

                break;
            }
            }
        }
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

    // TODO(log) outstanding resume data
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

void session::timer_callback()
{
    sess_->post_dht_stats();
    sess_->post_session_stats();
    sess_->post_torrent_updates();
}
