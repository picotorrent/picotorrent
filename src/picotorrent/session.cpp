#include "session.hpp"

#include <filesystem>
#include <iomanip>
#include <queue>

#include <QTimer>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/entry.hpp>
#include <libtorrent/extensions/smart_ban.hpp>
#include <libtorrent/extensions/ut_metadata.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_params.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/write_resume_data.hpp>

#include "loguru.hpp"

#include "core/configuration.hpp"
#include "core/database.hpp"
#include "core/environment.hpp"
#include "buildinfo.hpp"
#include "semver.hpp"
#include "sessionstatistics.hpp"
#include "torrenthandle.hpp"
#include "torrentstatistics.hpp"

namespace fs = std::filesystem;
namespace lt = libtorrent;

using pt::Session;

struct SessionLoadItem
{
    SessionLoadItem(fs::path const& p)
        : path(p)
    {
    }

    fs::path path;

    std::vector<char> resume_data;
    std::string magnet_save_path;
    std::string magnet_url;
};

static lt::session_params getSessionParams(std::shared_ptr<pt::Database> db)
{
    lt::session_params sp;

    auto stmt = db->statement("SELECT state_data FROM session_state ORDER BY timestamp DESC LIMIT 1");

    if (stmt->read())
    {
        std::vector<char> stateData;
        stmt->getBlob(0, stateData);

        lt::error_code ec;
        lt::bdecode_node node = lt::bdecode(stateData, ec);

        if (ec)
        {
            LOG_F(WARNING, "Failed to decode session state: %s", ec.message().data());
        }
        else
        {
            sp = lt::read_session_params(node, lt::session::save_dht_state);
        }
    }

    return sp;
}

static lt::settings_pack getSettingsPack(std::shared_ptr<pt::Configuration> cfg)
{
    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings.set_str(lt::settings_pack::string_types::dht_bootstrap_nodes,
        "router.bittorrent.com:6881" ","
        "router.utorrent.com:6881" ","
        "dht.transmissionbt.com:6881" ","
        "dht.aelitis.com:6881");

    std::string listenInterface = cfg->getString("listen_interface");
    int listenPort = cfg->getInt("listen_port");
    std::stringstream ifaces;

    // If the string starts with '{', i.e '{any}' or a network interface guid (which did not work),
    // set to listen to any interface.
    if (listenInterface.find_first_of('{') == 0)
    {
        ifaces << "0.0.0.0:" << listenPort << "," << "[::]:" << listenPort;
    }
    else
    {
        QString iface = QString::fromStdString(listenInterface);

        if (iface.contains("|"))
        {
            QStringList multi = iface.split("|");

            // Set up both IPv4 and IPv6
            ifaces << multi[0].toStdString() << ":" << listenPort << ",[" << multi[1].toStdString() << "]:" << listenPort;
        }
        else
        {
            // Only IPv4 here
            ifaces << listenInterface << ":" << listenPort;
        }
    }

    // Listen interface
    settings.set_str(lt::settings_pack::listen_interfaces, ifaces.str());

    // Features
    settings.set_bool(lt::settings_pack::enable_dht, cfg->getBool("enable_dht"));
    settings.set_bool(lt::settings_pack::enable_lsd, cfg->getBool("enable_lsd"));

    // Limits
    settings.set_int(lt::settings_pack::active_checking,      cfg->getInt("active_checking"));
    settings.set_int(lt::settings_pack::active_dht_limit,     cfg->getInt("active_dht_limit"));
    settings.set_int(lt::settings_pack::active_downloads,     cfg->getInt("active_downloads"));
    settings.set_int(lt::settings_pack::active_limit,         cfg->getInt("active_limit"));
    settings.set_int(lt::settings_pack::active_lsd_limit,     cfg->getInt("active_lsd_limit"));
    settings.set_int(lt::settings_pack::active_seeds,         cfg->getInt("active_seeds"));
    settings.set_int(lt::settings_pack::active_tracker_limit, cfg->getInt("active_tracker_limit"));

    // Encryption
    lt::settings_pack::enc_policy in_policy = cfg->getBool("require_incoming_encryption")
        ? lt::settings_pack::enc_policy::pe_forced
        : lt::settings_pack::enc_policy::pe_enabled;

    lt::settings_pack::enc_policy out_policy = cfg->getBool("require_outgoing_encryption")
        ? lt::settings_pack::enc_policy::pe_forced
        : lt::settings_pack::enc_policy::pe_enabled;

    settings.set_int(lt::settings_pack::int_types::in_enc_policy, in_policy);
    settings.set_int(lt::settings_pack::int_types::out_enc_policy, out_policy);

    // Various
    settings.set_bool(lt::settings_pack::anonymous_mode,      cfg->getBool("anonymous_mode"));
    settings.set_int(lt::settings_pack::stop_tracker_timeout, cfg->getInt("stop_tracker_timeout"));

    settings.set_int(lt::settings_pack::download_rate_limit,
        cfg->getBool("enable_download_rate_limit")
        ? cfg->getInt("download_rate_limit") * 1024
        : 0);

    settings.set_int(lt::settings_pack::upload_rate_limit,
        cfg->getBool("enable_upload_rate_limit")
        ? cfg->getInt("upload_rate_limit") * 1024
        : 0);

    // Calculate user agent
    std::stringstream user_agent;
    user_agent << "PicoTorrent/" << pt::BuildInfo::version().toStdString();

    // Calculate peer id
    semver::version v(pt::BuildInfo::version().toStdString());
    std::stringstream peer_id;
    peer_id << "-PI" << v.getMajor() << std::setfill('0') << std::setw(2) << v.getMinor() << v.getPatch() << "-";

    settings.set_str(lt::settings_pack::user_agent,       user_agent.str());
    settings.set_str(lt::settings_pack::peer_fingerprint, peer_id.str());

    // Proxy settings
    auto proxyType = static_cast<pt::Configuration::ConnectionProxyType>(cfg->getInt("proxy_type"));

    if (proxyType != pt::Configuration::ConnectionProxyType::None)
    {
        settings.set_int(lt::settings_pack::proxy_type,                 static_cast<lt::settings_pack::proxy_type_t>(proxyType));
        settings.set_str(lt::settings_pack::proxy_hostname,             cfg->getString("proxy_host"));
        settings.set_int(lt::settings_pack::proxy_port,                 cfg->getInt("proxy_port"));
        settings.set_str(lt::settings_pack::proxy_username,             cfg->getString("proxy_username"));
        settings.set_str(lt::settings_pack::proxy_password,             cfg->getString("proxy_password"));
        settings.set_bool(lt::settings_pack::proxy_hostnames,           cfg->getBool("proxy_hostnames"));
        settings.set_bool(lt::settings_pack::proxy_peer_connections,    cfg->getBool("proxy_peers"));
        settings.set_bool(lt::settings_pack::proxy_tracker_connections, cfg->getBool("proxy_trackers"));
    }

    return settings;
}

Session::Session(QObject* parent, std::shared_ptr<pt::Database> db, std::shared_ptr<pt::Configuration> cfg, std::shared_ptr<pt::Environment> env)
    : QObject(parent),
    m_db(db),
    m_cfg(cfg),
    m_env(env)
{
    lt::session_params sp = getSessionParams(db);
    sp.settings = getSettingsPack(cfg);

    m_session = std::make_unique<lt::session>(sp);
    m_session->add_extension(&lt::create_ut_metadata_plugin);
    m_session->add_extension(&lt::create_smart_ban_plugin);

    if (cfg->getBool("enable_pex"))
    {
        m_session->add_extension(lt::create_ut_pex_plugin);
    }

    this->loadTorrents();
    this->loadTorrentsOld();

    m_session->set_alert_notify([this]()
    {
        QMetaObject::invokeMethod(this, "readAlerts", Qt::QueuedConnection);
    });

    m_updateTimer = new QTimer(this);
    m_updateTimer->start(1000);

    QObject::connect(m_updateTimer, &QTimer::timeout,
                     this,          &Session::postUpdates);
}

Session::~Session()
{
    m_updateTimer->stop();
    m_session->set_alert_notify([]{});

    this->saveState();
    this->saveTorrents();
}

void Session::addTorrent(lt::add_torrent_params const& params)
{
    // If we are searching for metadata for this torrent, stop
    // that search and add this one instead.

    if (m_metadataSearches.find(params.info_hash) != m_metadataSearches.end())
    {
        lt::torrent_handle& hndl = m_metadataSearches.at(params.info_hash);

        // By default, an invalid torrent handle is added to the metadata
        // search map. Only remove the handle from the session if it has
        // been replaced with a valid one in the add_torrent_alert handler.

        if (hndl.is_valid())
        {
            m_session->remove_torrent(hndl, lt::session::delete_files);
        }
    }

    m_session->async_add_torrent(params);
}

void Session::metadataSearch(std::vector<lt::info_hash_t> const& hashes)
{
    // To do a metadata search (ie. find a torrent file based on its info hash)
    // we add the torrent with just the info_hash and save_path set, and then
    // track it in our alert loop.

    fs::path tmp = fs::temp_directory_path();

    for (lt::info_hash_t const& hash : hashes)
    {
        lt::add_torrent_params params;
        params.flags &= ~lt::torrent_flags::auto_managed;
        params.flags &= ~lt::torrent_flags::paused;
        params.flags &= ~lt::torrent_flags::update_subscribe;
        params.flags |=  lt::torrent_flags::upload_mode;

        params.info_hash = hash;
        params.save_path = tmp.string();

        // Track this info hash internally to make sure
        // we do not emit any events for it.
		m_metadataSearches.insert({ hash, lt::torrent_handle() });

        m_session->async_add_torrent(params);
    }
}

void Session::removeTorrent(pt::TorrentHandle* torrent, lt::remove_flags_t flags)
{
    m_session->remove_torrent(torrent->wrappedHandle(), flags);
}

void Session::reloadSettings()
{
    lt::settings_pack settings = getSettingsPack(m_cfg);
    m_session->apply_settings(settings);
}

void Session::loadTorrents()
{
    auto stmt = m_db->statement("SELECT t.info_hash AS info_hash, tmu.magnet_uri AS magnet_uri, trd.resume_data AS resume_data, tmu.save_path AS save_path FROM torrent t\n"
        "LEFT JOIN torrent_magnet_uri  tmu ON t.info_hash = tmu.info_hash\n"
        "LEFT JOIN torrent_resume_data trd ON t.info_hash = trd.info_hash\n"
        "ORDER BY t.queue_position ASC");

    while (stmt->read())
    {
        std::string info_hash  = stmt->getString(0);
        std::string magnet_uri = stmt->getString(1);
        std::string save_path  = stmt->getString(3);

        std::vector<char> resume_data;
        stmt->getBlob(2, resume_data);

        lt::add_torrent_params params;

        // Always parse magnet uri if it is empty
        if (!magnet_uri.empty())
        {
            params = lt::parse_magnet_uri(magnet_uri);
        }

        if (!save_path.empty())
        {
            params.save_path = save_path;
        }

        if (resume_data.size() > 0)
        {
            lt::error_code ec;
            lt::bdecode_node node = lt::bdecode(resume_data, ec);

            if (ec)
            {
                LOG_F(WARNING, "Failed to decode resume data: %s", ec.message().data());
                continue;
            }

            params = lt::read_resume_data(node, ec);

            if (ec)
            {
                LOG_F(WARNING, "Failed to read resume data: %s", ec.message().data());
                continue;
            }
        }

        m_session->async_add_torrent(params);
    }
}

void Session::loadTorrentsOld()
{
    fs::path dataDirectory = m_env->getApplicationDataPath();
    fs::path torrentsDirectory = dataDirectory / "Torrents";

    if (!fs::exists(torrentsDirectory))
    {
        return;
    }

    typedef std::pair<int64_t, SessionLoadItem> prio_item_t;
    auto comparer = [](const prio_item_t &lhs, const prio_item_t &rhs)
    {
        return lhs.first > rhs.first;
    };

    std::priority_queue<prio_item_t, std::vector<prio_item_t>, decltype(comparer)> queue(comparer);
    int64_t maxPosition = std::numeric_limits<int64_t>::max();

    for (auto& tmp : fs::directory_iterator(torrentsDirectory))
    {
        fs::path datFile = tmp.path();

        if (datFile.extension() != ".dat")
        {
            continue;
        }

        std::ifstream datStream(datFile, std::ios::binary | std::ios::in);
        
        SessionLoadItem item(datFile);
        std::stringstream ss;
        ss << datStream.rdbuf();
        std::string c = ss.str();
        item.resume_data.assign(c.begin(), c.end());

        lt::error_code ltec;
        lt::bdecode_node node = lt::bdecode(item.resume_data, ltec);

        if (ltec || node.type() != lt::bdecode_node::type_t::dict_t)
        {
            continue;
        }

        item.magnet_save_path = node.dict_find_string_value("pT-magnet-savePath").to_string();
        item.magnet_url = node.dict_find_string_value("pT-magnet-url").to_string();

        int64_t queuePosition = node.dict_find_int_value("pT-queuePosition", maxPosition);
        if (queuePosition < 0) { queuePosition = maxPosition; }

        queue.push({ queuePosition, item });
    }

    while (!queue.empty())
    {
        SessionLoadItem item = queue.top().second;
        queue.pop();

        fs::path torrent_file = fs::path(item.path).replace_extension(".torrent");

        if (!fs::exists(torrent_file)
            && item.magnet_url.empty())
        {
            fs::remove(torrent_file);
            continue;
        }

        lt::add_torrent_params params;

        if (!item.resume_data.empty())
        {
            lt::error_code ltec;
            params = lt::read_resume_data(
                item.resume_data,
                ltec);
        }

        if (fs::exists(torrent_file))
        {

            std::ifstream torrent_input(torrent_file, std::ios::binary);
            std::stringstream ss;
            ss << torrent_input.rdbuf();
            std::string torrent_buf = ss.str();

            lt::bdecode_node node;
            lt::error_code ltec;
            lt::bdecode(
                &torrent_buf[0],
                &torrent_buf[0] + torrent_buf.size(),
                node,
                ltec);

            if (ltec)
            {
                continue;
            }

            params.ti = std::make_shared<lt::torrent_info>(node);
        }

        if (!item.magnet_url.empty())
        {
            lt::error_code ec;
            lt::parse_magnet_uri(item.magnet_url, params, ec);
            params.save_path = item.magnet_save_path;
        }

        m_session->async_add_torrent(params);

        // Remove torrent and dat file
        std::error_code fec;
        if (fs::exists(item.path, fec)) fs::remove(item.path, fec);
        if (fs::exists(torrent_file, fec)) fs::remove(torrent_file, fec);
    }

    // if torrents dir is empty, remove it
    std::error_code dec;
    if (fs::is_empty(torrentsDirectory, dec))
    {
        fs::remove(torrentsDirectory, dec);
    }
}

void Session::postUpdates()
{
    m_session->post_dht_stats();
    m_session->post_session_stats();
    m_session->post_torrent_updates();
}

void Session::readAlerts()
{
    std::vector<lt::alert*> alerts;
    m_session->pop_alerts(&alerts);

    for (lt::alert* alert : alerts)
    {
        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            lt::add_torrent_alert* ata = lt::alert_cast<lt::add_torrent_alert>(alert);

            if (ata->error)
            {
                LOG_F(ERROR, "Failed to add torrent to session: %s", ata->error.message().data());
                continue;
            }

            if (m_metadataSearches.count(ata->handle.info_hash()))
            {
                // Part of a metadata search - update the metadata search map
                // with the new handle, then ignore it.
                m_metadataSearches[ata->handle.info_hash()] = ata->handle;
                continue;
            }

            std::stringstream ss;

            if (ata->handle.info_hash().has_v2())
            {
                ss << ata->handle.info_hash().v2;
            }
            else
            {
                ss << ata->handle.info_hash().v1;
            }

            std::string ih = ss.str();

            lt::torrent_status ts = ata->handle.status();

            auto stmt = m_db->statement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
            stmt->bind(1, ih);
            stmt->bind(2, static_cast<int>(ts.queue_position));
            stmt->execute();

            ata->handle.save_resume_data(
                lt::torrent_handle::flush_disk_cache
                | lt::torrent_handle::save_info_dict);

            auto handle = new TorrentHandle(this, ata->handle);

            m_torrents.insert({ ata->handle.info_hash(), handle });

            emit torrentAdded(handle);

            break;
        }

        case lt::metadata_received_alert::alert_type:
        {
            lt::metadata_received_alert* mra = lt::alert_cast<lt::metadata_received_alert>(alert);
            lt::info_hash_t infoHash = mra->handle.info_hash();

            if (m_metadataSearches.count(infoHash) > 0)
            {
                // Create a non-const copy of the torrent_info

                auto tiConst = mra->handle.torrent_file();
                auto ti = std::make_shared<lt::torrent_info>(*tiConst.get());

                emit metadataSearchResult(&ti);

                m_session->remove_torrent(mra->handle, lt::session::delete_files);
            }
            else
            {
                std::stringstream ss;

                if (infoHash.has_v2())
                {
                    ss << infoHash.v2;
                }
                else
                {
                    ss << infoHash.v1;
                }

                // if we recieve the metadata for a torrent we are currently interested in,
                // we can remove the magnet_uri stored for it
                {
                    auto stmt = m_db->statement("DELETE FROM torrent_magnet_uri  WHERE info_hash = ?;");
                    stmt->bind(1, ss.str());
                    stmt->execute();
                }

                if (mra->handle.need_save_resume_data())
                {
                    mra->handle.save_resume_data(
                        lt::torrent_handle::flush_disk_cache
                        | lt::torrent_handle::save_info_dict);
                }
            }

            break;
        }

        case lt::save_resume_data_alert::alert_type:
        {
            lt::save_resume_data_alert* srda = lt::alert_cast<lt::save_resume_data_alert>(alert);
            std::vector<char> buffer = lt::write_resume_data_buf(srda->params);

            std::stringstream ss;

            if (srda->handle.info_hash().has_v2())
            {
                ss << srda->handle.info_hash().v2;
            }
            else
            {
                ss << srda->handle.info_hash().v1;
            }

            std::string ih = ss.str();

            // Store the data
            auto stmt = m_db->statement("REPLACE INTO torrent_resume_data (info_hash, resume_data) VALUES (?, ?);");
            stmt->bind(1, ih);
            stmt->bind(2, buffer);
            stmt->execute();

            break;
        }

        case lt::session_stats_alert::alert_type:
        {
            lt::session_stats_alert* ssa = lt::alert_cast<lt::session_stats_alert>(alert);
            lt::span<const int64_t> counters = ssa->counters();
            int idx = -1;

            SessionStatistics stats;

            if ((idx = lt::find_metric_idx("dht.dht_nodes")) >= 0)
            {
                stats.dhtNodes = counters[idx];
            }

            emit sessionStatsUpdated(&stats);

            break;
        }

        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);

            TorrentStatistics stats = { 0 };

            for (lt::torrent_status const& status : sua->status)
            {
                // Skip torrents which are not found in m_torrents - this can happen
                // when we recieve alerts for a torrent currently in metadata search
                if (m_torrents.find(status.info_hash) == m_torrents.end())
                {
                    continue;
                }

                stats.totalPayloadDownloadRate += status.download_payload_rate;
                stats.totalPayloadUploadRate   += status.upload_payload_rate;

                if (status.state == lt::torrent_status::state_t::downloading)
                {
                    stats.totalWanted     += status.total_wanted;
                    stats.totalWantedDone += status.total_wanted_done;
                }

                auto handle = m_torrents.at(status.info_hash);
                handle->updateStatus(status);
                emit torrentUpdated(handle);
            }

            emit torrentStatsUpdated(&stats);

            break;
        }

        case lt::torrent_finished_alert::alert_type:
        {
            lt::torrent_finished_alert* tfa = lt::alert_cast<lt::torrent_finished_alert>(alert);
            lt::torrent_status const& ts    = tfa->handle.status();

            // Only move from completed path if we have downloaded any payload
            // bytes, otherwise it's most likely a newly added torrent which we
            // had already downloaded.

            if (ts.total_payload_download <= 0)
            {
                break;
            }

            emit torrentFinished(m_torrents.at(ts.info_hash));

            bool shouldMove      = m_cfg->getBool("move_completed_downloads");
            bool onlyFromDefault = m_cfg->getBool("move_completed_downloads_from_default_only");
            std::string movePath = m_cfg->getString("move_completed_downloads_path");

            if (shouldMove)
            {
                if (onlyFromDefault && ts.save_path != m_cfg->getString("default_save_path"))
                {
                    break;
                }

                tfa->handle.move_storage(movePath);
            }

            break;
        }

        case lt::torrent_removed_alert::alert_type:
        {
            lt::torrent_removed_alert* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);

            if (m_metadataSearches.count(tra->info_hash) > 0)
            {
                m_metadataSearches.erase(tra->info_hash);
                break;
            }

            auto handle = m_torrents.at(tra->info_hash);

            emit torrentRemoved(handle);

            m_torrents.erase(tra->info_hash);

            std::vector<std::string> statements =
            {
                "DELETE FROM torrent_resume_data WHERE info_hash = ?;",
                "DELETE FROM torrent_magnet_uri  WHERE info_hash = ?;",
                "DELETE FROM torrent             WHERE info_hash = ?;",
            };

            std::stringstream hash;

            if (tra->info_hash.has_v2())
            {
                hash << tra->info_hash.v2;
            }
            else
            {
                hash << tra->info_hash.v1;
            }

            for (std::string const& sql : statements)
            {
                auto stmt = m_db->statement(sql);
                stmt->bind(1, hash.str());
                stmt->execute();
            }

            handle->deleteLater();

            break;
        }
        }
    }
}

void Session::saveState()
{
    std::vector<char> stateBuffer = lt::write_session_params_buf(
        m_session->session_state(),
        lt::session::save_dht_state);

    auto stmt = m_db->statement("INSERT INTO session_state (state_data, timestamp) VALUES (?, strftime('%s'))");
    stmt->bind(1, stateBuffer);
    stmt->execute();

    // Keep only the five last states
    m_db->execute("DELETE FROM session_state WHERE id NOT IN (SELECT id FROM session_state ORDER BY timestamp DESC LIMIT 5)");
}

void Session::saveTorrents()
{
    m_session->pause();

    // Save each torrents resume data
    int numOutstandingResumeData = 0;
    int numPaused = 0;
    int numFailed = 0;

    // Save all torrents not part of a metadata search
    auto temp = m_session->get_torrent_status(
        [this](const lt::torrent_status &st)
        {
            return m_metadataSearches.count(st.info_hash) == 0;
        });

    for (lt::torrent_status &st : temp)
    {
        if (!st.handle.is_valid()
            || !st.has_metadata
            || !st.need_save_resume)
        {
            continue;
        }

        st.handle.save_resume_data(
            lt::torrent_handle::flush_disk_cache
            | lt::torrent_handle::save_info_dict);

        ++numOutstandingResumeData;
    }

    // Save all torrents without metadata
    auto missingMeta = m_session->get_torrent_status(
        [this](const lt::torrent_status& st)
        {
            return !st.has_metadata;
        });

    LOG_F(INFO, "Saving data for %d torrent(s)", numOutstandingResumeData + static_cast<int>(missingMeta.size()));

    for (lt::torrent_status& st : missingMeta)
    {
        std::stringstream ss;

        if (st.info_hash.has_v2())
        {
            ss << st.info_hash.v2;
        }
        else
        {
            ss << st.info_hash.v1;
        }

        // Store state
        auto stmt = m_db->statement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
        stmt->bind(1, ss.str());
        stmt->bind(2, static_cast<int>(st.queue_position));
        stmt->execute();

        // Store the magnet uri
        stmt = m_db->statement("REPLACE INTO torrent_magnet_uri (info_hash, magnet_uri, save_path) VALUES (?, ?, ?);");
        stmt->bind(1, ss.str());
        stmt->bind(2, lt::make_magnet_uri(st.handle));
        stmt->bind(3, st.handle.status(lt::torrent_handle::query_save_path).save_path);
        stmt->execute();
    }

    while (numOutstandingResumeData > 0)
    {
        lt::alert const* a = m_session->wait_for_alert(lt::seconds(10));
        if (a == nullptr) { continue; }

        std::vector<lt::alert*> alerts;
        m_session->pop_alerts(&alerts);

        for (lt::alert *a : alerts)
        {
            lt::torrent_paused_alert *tp = lt::alert_cast<lt::torrent_paused_alert>(a);

            if (tp)
            {
                ++numPaused;
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++numFailed;
                --numOutstandingResumeData;
                continue;
            }

            lt::save_resume_data_alert *rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --numOutstandingResumeData;

            std::vector<char> buffer = lt::write_resume_data_buf(rd->params);

            std::stringstream ss;

            if (rd->handle.info_hash().has_v2())
            {
                ss << rd->handle.info_hash().v2;
            }
            else
            {
                ss << rd->handle.info_hash().v1;
            }

            std::string ih = ss.str();

            // Store state
            auto stmt = m_db->statement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
            stmt->bind(1, ih);
            stmt->bind(2, static_cast<int>(rd->handle.status().queue_position));
            stmt->execute();

            // Store the data
            stmt = m_db->statement("REPLACE INTO torrent_resume_data (info_hash, resume_data) VALUES (?, ?);");
            stmt->bind(1, ih);
            stmt->bind(2, buffer);
            stmt->execute();
        }
    }
}
