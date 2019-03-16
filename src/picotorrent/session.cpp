#include "session.hpp"

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
#include <libtorrent/session_stats.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/write_resume_data.hpp>

#include <picotorrent/core/configuration.hpp>
#include <picotorrent/core/database.hpp>

#include "buildinfo.hpp"
#include "semver.hpp"
#include "sessionstatistics.hpp"
#include "torrenthandle.hpp"
#include "torrentstatistics.hpp"

namespace lt = libtorrent;
using pt::Session;

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

    if (listenInterface == "{any}")
    {
        ifaces << "0.0.0.0:" << listenPort << "," << "[::]:" << listenPort;
    }
    else
    {
        ifaces << listenInterface << "::" << listenPort;
    }

    // Listen interface
    settings.set_str(lt::settings_pack::listen_interfaces, ifaces.str());

    // Features
    settings.set_bool(lt::settings_pack::enable_dht, cfg->getBool("enable_dht"));
    settings.set_bool(lt::settings_pack::enable_lsd, cfg->getBool("enable_lsd"));

    // Limits
    settings.set_int(lt::settings_pack::active_checking,      cfg->getBool("active_checking"));
    settings.set_int(lt::settings_pack::active_dht_limit,     cfg->getBool("active_dht_limit"));
    settings.set_int(lt::settings_pack::active_downloads,     cfg->getBool("active_downloads"));
    settings.set_int(lt::settings_pack::active_limit,         cfg->getBool("active_limit"));
    settings.set_int(lt::settings_pack::active_lsd_limit,     cfg->getBool("active_lsd_limit"));
    settings.set_int(lt::settings_pack::active_seeds,         cfg->getBool("active_seeds"));
    settings.set_int(lt::settings_pack::active_tracker_limit, cfg->getBool("active_tracker_limit"));

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
        ? cfg->getBool("download_rate_limit") * 1024
        : 0);

    settings.set_int(lt::settings_pack::upload_rate_limit,
        cfg->getBool("enable_upload_rate_limit")
        ? cfg->getBool("upload_rate_limit") * 1024
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
        settings.set_bool(lt::settings_pack::proxy_peer_connections,    cfg->getBool("proxy_peer_connections"));
        settings.set_bool(lt::settings_pack::proxy_tracker_connections, cfg->getBool("proxy_tracker_connections"));
    }

    return settings;
}

Session::Session(QObject* parent, std::shared_ptr<pt::Database> db, std::shared_ptr<pt::Configuration> cfg)
    : QObject(parent),
    m_db(db),
    m_cfg(cfg)
{
    lt::settings_pack settings = getSettingsPack(cfg);

    m_session = std::make_unique<lt::session>(
        settings,
        lt::session_flags_t { lt::session_handle::add_default_plugins });

    if (cfg->getBool("enable_pex"))
    {
        m_session->add_extension(lt::create_ut_pex_plugin);
    }

    this->loadState();
    this->loadTorrents();

    m_session->set_alert_notify([this]()
    {
        QMetaObject::invokeMethod(this, "readAlerts", Qt::QueuedConnection);
    });

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &Session::postUpdates);
    m_updateTimer->start(1000);
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
    m_session->async_add_torrent(params);
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

void Session::loadState()
{
    auto stmt = m_db->statement("SELECT state_data FROM session_state ORDER BY timestamp DESC LIMIT 1");

    if (stmt->read())
    {
        std::vector<char> stateData;
        stmt->getBlob(0, stateData);

        lt::error_code ec;
        lt::bdecode_node node = lt::bdecode(stateData, ec);

        if (ec)
        {
            // TODO(log)
        }
        else
        {
            m_session->load_state(node);
        }
    }
}

void Session::loadTorrents()
{
    auto stmt = m_db->statement("SELECT trd.resume_data AS resume_data FROM torrent t\n"
        "LEFT JOIN torrent_resume_data trd ON t.info_hash = trd.info_hash\n"
        "ORDER BY t.queue_position ASC");

    while (stmt->read())
    {
        std::vector<char> resume_data;
        stmt->getBlob(0, resume_data);

        lt::add_torrent_params params;

        if (resume_data.size() > 0)
        {
            lt::error_code ec;
            lt::bdecode_node node = lt::bdecode(resume_data, ec);

            if (ec)
            {
                // TODO(log)
            }

            params = lt::read_resume_data(node, ec);

            if (ec)
            {
                // TODO(log)
            }
        }

        m_session->async_add_torrent(params);
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
                // TODO(log)
                continue;
            }

            std::stringstream ss;
            ss << ata->handle.info_hash();
            std::string ih = ss.str();

            lt::torrent_status ts = ata->handle.status();

            auto stmt = m_db->statement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
            stmt->bind(1, ih);
            stmt->bind(2, static_cast<int>(ts.queue_position));
            stmt->execute();

            ata->handle.save_resume_data(
                lt::torrent_handle::flush_disk_cache
                | lt::torrent_handle::save_info_dict);

            /*m_sessionState->torrents.insert({ ts.info_hash, ts.handle });
            m_statusBar->updateTorrentCount(m_sessionState->torrents.size());
            m_torrentListModel->addTorrent(ts);*/

            auto handle = new TorrentHandle(this, ata->handle);

            m_torrents.insert({ ata->handle.info_hash(), handle });

            emit torrentAdded(handle);

            break;
        }

        case lt::save_resume_data_alert::alert_type:
        {
            lt::save_resume_data_alert* srda = lt::alert_cast<lt::save_resume_data_alert>(alert);
            std::vector<char> buffer = lt::write_resume_data_buf(srda->params);

            std::stringstream ss;
            ss << srda->handle.info_hash();
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
                stats.totalPayloadDownloadRate += status.download_payload_rate;
                stats.totalPayloadUploadRate   += status.upload_payload_rate;

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

            auto handle = m_torrents.at(tra->info_hash);
            m_torrents.erase(tra->info_hash);

            emit torrentRemoved(handle);

            std::vector<std::string> statements =
            {
                "DELETE FROM torrent_resume_data WHERE info_hash = ?;",
                "DELETE FROM torrent             WHERE info_hash = ?;",
            };

            std::stringstream hash;
            hash << tra->info_hash;

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
    // Save session state
    lt::entry entry;
    m_session->save_state(entry);

    std::vector<char> stateBuffer;
    lt::bencode(std::back_inserter(stateBuffer), entry);

    auto stmt = m_db->statement("INSERT INTO session_state (state_data, timestamp) VALUES (?, strftime('%s'))");
    stmt->bind(1, stateBuffer);
    stmt->execute();
}

void Session::saveTorrents()
{
    m_session->pause();

    // Save each torrents resume data
    int numOutstandingResumeData = 0;
    int numPaused = 0;
    int numFailed = 0;

    auto temp = m_session->get_torrent_status([](const lt::torrent_status &st) { return true; });

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
            ss << rd->handle.info_hash();
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
