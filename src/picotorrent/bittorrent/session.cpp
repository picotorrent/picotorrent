#include "session.hpp"

#include <filesystem>
#include <queue>

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
#include <loguru.hpp>

#include "../core/configuration.hpp"
#include "../core/database.hpp"
#include "../core/environment.hpp"
#include "../buildinfo.hpp"
#include "semver.hpp"
#include "sessionstatistics.hpp"
#include "torrenthandle.hpp"
#include "torrentstatistics.hpp"

namespace fs = std::filesystem;
namespace lt = libtorrent;
using pt::BitTorrent::Session;

wxDEFINE_EVENT(ptEVT_SESSION_DEBUG_MESSAGE, wxCommandEvent);
wxDEFINE_EVENT(ptEVT_SESSION_STATISTICS, pt::BitTorrent::SessionStatisticsEvent);
wxDEFINE_EVENT(ptEVT_TORRENT_ADDED, wxCommandEvent);
wxDEFINE_EVENT(ptEVT_TORRENT_FINISHED, wxCommandEvent);
wxDEFINE_EVENT(ptEVT_TORRENT_METADATA_FOUND, pt::BitTorrent::MetadataFoundEvent);
wxDEFINE_EVENT(ptEVT_TORRENT_REMOVED, pt::BitTorrent::InfoHashEvent);
wxDEFINE_EVENT(ptEVT_TORRENT_STATISTICS, pt::BitTorrent::TorrentStatisticsEvent);
wxDEFINE_EVENT(ptEVT_TORRENTS_UPDATED, pt::BitTorrent::TorrentsUpdatedEvent);

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

static lt::session_params getSessionParams(std::shared_ptr<pt::Core::Database> db)
{
    lt::session_params sp;

    auto stmt = db->CreateStatement("SELECT state_data FROM session_state ORDER BY timestamp DESC LIMIT 1");

    if (stmt->Read())
    {
        std::vector<char> stateData;
        stmt->GetBlob(0, stateData);

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

static lt::settings_pack getSettingsPack(std::shared_ptr<pt::Core::Configuration> cfg)
{
    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);

    std::stringstream dhtNodes;
    std::stringstream ifaces;

    for (auto const& node : cfg->GetDhtBootstrapNodes())
    {
        dhtNodes << "," << node.hostname << ":" << node.port;
    }

    for (auto const& li : cfg->GetListenInterfaces())
    {
        ifaces << "," << li.address << ":" << li.port;
    }

    settings.set_str(lt::settings_pack::dht_bootstrap_nodes, dhtNodes.str().substr(1));
    settings.set_str(lt::settings_pack::listen_interfaces, ifaces.str().substr(1));

    // Features
    settings.set_bool(lt::settings_pack::enable_dht, cfg->GetBool("enable_dht"));
    settings.set_bool(lt::settings_pack::enable_lsd, cfg->GetBool("enable_lsd"));

    // Limits
    settings.set_int(lt::settings_pack::active_checking, cfg->GetInt("active_checking"));
    settings.set_int(lt::settings_pack::active_dht_limit, cfg->GetInt("active_dht_limit"));
    settings.set_int(lt::settings_pack::active_downloads, cfg->GetInt("active_downloads"));
    settings.set_int(lt::settings_pack::active_limit, cfg->GetInt("active_limit"));
    settings.set_int(lt::settings_pack::active_lsd_limit, cfg->GetInt("active_lsd_limit"));
    settings.set_int(lt::settings_pack::active_seeds, cfg->GetInt("active_seeds"));
    settings.set_int(lt::settings_pack::active_tracker_limit, cfg->GetInt("active_tracker_limit"));

    // Encryption
    lt::settings_pack::enc_policy in_policy = cfg->GetBool("require_incoming_encryption")
        ? lt::settings_pack::enc_policy::pe_forced
        : lt::settings_pack::enc_policy::pe_enabled;

    lt::settings_pack::enc_policy out_policy = cfg->GetBool("require_outgoing_encryption")
        ? lt::settings_pack::enc_policy::pe_forced
        : lt::settings_pack::enc_policy::pe_enabled;

    settings.set_int(lt::settings_pack::int_types::in_enc_policy, in_policy);
    settings.set_int(lt::settings_pack::int_types::out_enc_policy, out_policy);

    // Various
    settings.set_bool(lt::settings_pack::anonymous_mode, cfg->GetBool("anonymous_mode"));
    settings.set_int(lt::settings_pack::stop_tracker_timeout, cfg->GetInt("stop_tracker_timeout"));

    settings.set_int(lt::settings_pack::download_rate_limit,
        cfg->GetBool("enable_download_rate_limit")
        ? cfg->GetInt("download_rate_limit") * 1024
        : 0);

    settings.set_int(lt::settings_pack::upload_rate_limit,
        cfg->GetBool("enable_upload_rate_limit")
        ? cfg->GetInt("upload_rate_limit") * 1024
        : 0);

    // Calculate user agent
    std::stringstream user_agent;
    user_agent << "PicoTorrent/" << pt::BuildInfo::version();

    // Calculate peer id
    semver::version v(pt::BuildInfo::version());
    std::stringstream peer_id;
    peer_id << "-PI" << v.getMajor() << std::setfill('0') << std::setw(2) << v.getMinor() << v.getPatch() << "-";

    settings.set_str(lt::settings_pack::user_agent, user_agent.str());
    settings.set_str(lt::settings_pack::peer_fingerprint, peer_id.str());

    // Proxy settings
    auto proxyType = static_cast<pt::Core::Configuration::ConnectionProxyType>(cfg->GetInt("proxy_type"));

    if (proxyType != pt::Core::Configuration::ConnectionProxyType::None)
    {
        settings.set_int(lt::settings_pack::proxy_type, static_cast<lt::settings_pack::proxy_type_t>(proxyType));
        settings.set_str(lt::settings_pack::proxy_hostname, cfg->GetString("proxy_host"));
        settings.set_int(lt::settings_pack::proxy_port, cfg->GetInt("proxy_port"));
        settings.set_str(lt::settings_pack::proxy_username, cfg->GetString("proxy_username"));
        settings.set_str(lt::settings_pack::proxy_password, cfg->GetString("proxy_password"));
        settings.set_bool(lt::settings_pack::proxy_hostnames, cfg->GetBool("proxy_hostnames"));
        settings.set_bool(lt::settings_pack::proxy_peer_connections, cfg->GetBool("proxy_peers"));
        settings.set_bool(lt::settings_pack::proxy_tracker_connections, cfg->GetBool("proxy_trackers"));
    }

    return settings;
}

Session::Session(wxEvtHandler* parent, std::shared_ptr<pt::Core::Database> db, std::shared_ptr<pt::Core::Configuration> cfg, std::shared_ptr<pt::Core::Environment> env)
    : m_parent(parent),
    m_timer(new wxTimer(this)),
    m_cfg(cfg),
    m_db(db),
    m_env(env)
{
    lt::session_params sp = getSessionParams(db);
    sp.settings = getSettingsPack(cfg);

    m_session = std::make_unique<lt::session>(sp);
    m_session->add_extension(&lt::create_ut_metadata_plugin);
    m_session->add_extension(&lt::create_smart_ban_plugin);

    if (cfg->GetBool("enable_pex"))
    {
        m_session->add_extension(lt::create_ut_pex_plugin);
    }

    m_session->set_alert_notify(
        [this]
        {
            this->CallAfter(std::bind(&Session::OnAlert, this));
        });

    this->LoadTorrents();
    this->LoadTorrentsOld();

    m_timer->Start(1000, wxTIMER_CONTINUOUS);

    this->Bind(wxEVT_TIMER, [this](wxTimerEvent&)
        {
            m_session->post_dht_stats();
            m_session->post_session_stats();
            m_session->post_torrent_updates();
        });
}

Session::~Session()
{
    m_session->set_alert_notify([] {});
    m_timer->Stop();

    this->SaveState();
    this->SaveTorrents();
}

void Session::AddMetadataSearch(std::vector<libtorrent::info_hash_t> const& hashes)
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
        params.flags |= lt::torrent_flags::upload_mode;

        params.info_hash = hash;
        params.save_path = tmp.string();

        // Track this info hash internally to make sure
        // we do not emit any events for it.
        m_metadataSearches.insert({ hash, lt::torrent_handle() });

        m_session->async_add_torrent(params);
    }
}

void Session::AddTorrent(lt::add_torrent_params const& params)
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

void Session::RemoveTorrent(pt::BitTorrent::TorrentHandle* torrent, lt::remove_flags_t flags)
{
    m_session->remove_torrent(torrent->WrappedHandle(), flags);
}

void Session::ReloadSettings()
{
    lt::settings_pack settings = getSettingsPack(m_cfg);
    m_session->apply_settings(settings);
}

void Session::OnAlert()
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

            auto stmt = m_db->CreateStatement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
            stmt->Bind(1, ih);
            stmt->Bind(2, static_cast<int>(ts.queue_position));
            stmt->Execute();

            ata->handle.save_resume_data(
                lt::torrent_handle::flush_disk_cache
                | lt::torrent_handle::save_info_dict);

            auto handle = new TorrentHandle(this, ata->handle);

            m_torrents.insert({ ata->handle.info_hash(), handle });

            wxCommandEvent torrentAdded(ptEVT_TORRENT_ADDED);
            torrentAdded.SetClientData(handle);
            wxPostEvent(m_parent, torrentAdded);

            break;
        }

        case lt::file_error_alert::alert_type:
        {
            lt::file_error_alert* fea = lt::alert_cast<lt::file_error_alert>(alert);

            auto torrent = m_torrents.at(fea->handle.info_hash());

            TorrentsUpdatedEvent evtUpdated(ptEVT_TORRENTS_UPDATED);
            evtUpdated.SetData({ torrent });
            wxPostEvent(m_parent, evtUpdated);

            break;
        }

        case lt::listen_failed_alert::alert_type:
        {
            LOG_F(WARNING, alert->message().c_str());
            break;
        }

        case lt::listen_succeeded_alert::alert_type:
        {
            LOG_F(INFO, alert->message().c_str());
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

                MetadataFoundEvent evt(ptEVT_TORRENT_METADATA_FOUND);
                evt.SetData(ti);
                wxPostEvent(this, evt);
                wxPostEvent(m_parent, evt);

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

            {
                // Store the data
                auto stmt = m_db->CreateStatement("REPLACE INTO torrent_resume_data (info_hash, resume_data) VALUES (?, ?);");
                stmt->Bind(1, ih);
                stmt->Bind(2, buffer);
                stmt->Execute();
            }

            {
                // at this stage we can remove the magnet link
                auto stmt = m_db->CreateStatement("DELETE FROM torrent_magnet_uri  WHERE info_hash = ?;");
                stmt->Bind(1, ss.str());
                stmt->Execute();
            }

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

            SessionStatisticsEvent evt(ptEVT_SESSION_STATISTICS);
            evt.SetData(stats);
            wxPostEvent(m_parent, evt);

            break;
        }

        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);

            std::vector<TorrentHandle*> handles;
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
                stats.totalPayloadUploadRate += status.upload_payload_rate;

                if (status.state == lt::torrent_status::state_t::downloading)
                {
                    stats.isDownloadingAny = true;
                    stats.totalWanted += status.total_wanted;
                    stats.totalWantedDone += status.total_wanted_done;
                }

                auto handle = m_torrents.at(status.info_hash);
                handle->UpdateStatus(status);

                handles.push_back(handle);
            }

            TorrentStatisticsEvent evt(ptEVT_TORRENT_STATISTICS);
            evt.SetData(stats);
            wxPostEvent(m_parent, evt);

            TorrentsUpdatedEvent evtUpdated(ptEVT_TORRENTS_UPDATED);
            evtUpdated.SetData(handles);
            wxPostEvent(m_parent, evtUpdated);

            break;
        }

        case lt::torrent_checked_alert::alert_type:
        {
            lt::torrent_checked_alert* tca = lt::alert_cast<lt::torrent_checked_alert>(alert);

            auto torrentToResume = m_pauseAfterRecheck.find(tca->handle.info_hash());

            if (torrentToResume != m_pauseAfterRecheck.end())
            {
                torrentToResume->second->Pause();
                m_pauseAfterRecheck.erase(torrentToResume);
            }

            break;
        }

        case lt::torrent_error_alert::alert_type:
        {
            LOG_F(ERROR, "Torrent error: %s", alert->message().c_str());
            break;
        }

        case lt::torrent_finished_alert::alert_type:
        {
            lt::torrent_finished_alert* tfa = lt::alert_cast<lt::torrent_finished_alert>(alert);
            lt::torrent_status const& ts = tfa->handle.status();

            // Only move from completed path if we have downloaded any payload
            // bytes, otherwise it's most likely a newly added torrent which we
            // had already downloaded.

            if (ts.total_payload_download <= 0)
            {
                break;
            }

            wxCommandEvent evt(ptEVT_TORRENT_FINISHED);
            evt.SetClientData(m_torrents.at(ts.info_hash));
            wxPostEvent(m_parent, evt);

            bool shouldMove = m_cfg->GetBool("move_completed_downloads");
            bool onlyFromDefault = m_cfg->GetBool("move_completed_downloads_from_default_only");
            std::string movePath = m_cfg->GetString("move_completed_downloads_path");

            if (shouldMove)
            {
                if (onlyFromDefault && ts.save_path != m_cfg->GetString("default_save_path"))
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

            InfoHashEvent evt(ptEVT_TORRENT_REMOVED);
            evt.SetData(tra->info_hash);
            wxPostEvent(m_parent, evt);

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
                auto stmt = m_db->CreateStatement(sql);
                stmt->Bind(1, hash.str());
                stmt->Execute();
            }

            // TODO: check if corrrect
            delete handle;

            break;
        }
        }
    }
}

void Session::LoadTorrents()
{
    auto stmt = m_db->CreateStatement("SELECT t.info_hash AS info_hash, tmu.magnet_uri AS magnet_uri, trd.resume_data AS resume_data, tmu.save_path AS save_path FROM torrent t\n"
        "LEFT JOIN torrent_magnet_uri  tmu ON t.info_hash = tmu.info_hash\n"
        "LEFT JOIN torrent_resume_data trd ON t.info_hash = trd.info_hash\n"
        "ORDER BY t.queue_position ASC");

    while (stmt->Read())
    {
        std::string info_hash = stmt->GetString(0);
        std::string magnet_uri = stmt->GetString(1);
        std::string save_path = stmt->GetString(3);

        std::vector<char> resume_data;
        stmt->GetBlob(2, resume_data);

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

void Session::LoadTorrentsOld()
{
    fs::path dataDirectory = m_env->GetApplicationDataPath();
    fs::path torrentsDirectory = dataDirectory / "Torrents";

    if (!fs::exists(torrentsDirectory))
    {
        return;
    }

    typedef std::pair<int64_t, SessionLoadItem> prio_item_t;
    auto comparer = [](const prio_item_t& lhs, const prio_item_t& rhs)
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

void Session::PauseAfterRecheck(pt::BitTorrent::TorrentHandle* th)
{
    if (m_pauseAfterRecheck.find(th->InfoHash()) != m_pauseAfterRecheck.end())
    {
        LOG_F(WARNING, "Torrent already rechecking (%s)", th->InfoHash().v1.data());
        return;
    }

    m_pauseAfterRecheck.insert({ th->InfoHash(), th });
}

void Session::SaveState()
{
    std::vector<char> stateBuffer = lt::write_session_params_buf(
        m_session->session_state(),
        lt::session::save_dht_state);

    auto stmt = m_db->CreateStatement("INSERT INTO session_state (state_data, timestamp) VALUES (?, strftime('%s'))");
    stmt->Bind(1, stateBuffer);
    stmt->Execute();

    // Keep only the five last states
    m_db->Execute("DELETE FROM session_state WHERE id NOT IN (SELECT id FROM session_state ORDER BY timestamp DESC LIMIT 5)");
}

void Session::SaveTorrents()
{
    m_session->pause();

    // Save each torrents resume data
    int numOutstandingResumeData = 0;
    int numPaused = 0;
    int numFailed = 0;

    // Save all torrents not part of a metadata search
    auto temp = m_session->get_torrent_status(
        [this](const lt::torrent_status& st)
        {
            return m_metadataSearches.count(st.info_hash) == 0;
        });

    for (lt::torrent_status& st : temp)
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
        auto stmt = m_db->CreateStatement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
        stmt->Bind(1, ss.str());
        stmt->Bind(2, static_cast<int>(st.queue_position));
        stmt->Execute();

        // Store the magnet uri
        stmt = m_db->CreateStatement("REPLACE INTO torrent_magnet_uri (info_hash, magnet_uri, save_path) VALUES (?, ?, ?);");
        stmt->Bind(1, ss.str());
        stmt->Bind(2, lt::make_magnet_uri(st.handle));
        stmt->Bind(3, st.handle.status(lt::torrent_handle::query_save_path).save_path);
        stmt->Execute();
    }

    while (numOutstandingResumeData > 0)
    {
        lt::alert const* a = m_session->wait_for_alert(lt::seconds(10));
        if (a == nullptr) { continue; }

        std::vector<lt::alert*> alerts;
        m_session->pop_alerts(&alerts);

        for (lt::alert* a : alerts)
        {
            lt::torrent_paused_alert* tp = lt::alert_cast<lt::torrent_paused_alert>(a);

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

            lt::save_resume_data_alert* rd = lt::alert_cast<lt::save_resume_data_alert>(a);
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
            auto stmt = m_db->CreateStatement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
            stmt->Bind(1, ih);
            stmt->Bind(2, static_cast<int>(rd->handle.status().queue_position));
            stmt->Execute();

            // Store the data
            stmt = m_db->CreateStatement("REPLACE INTO torrent_resume_data (info_hash, resume_data) VALUES (?, ?);");
            stmt->Bind(1, ih);
            stmt->Bind(2, buffer);
            stmt->Execute();
        }
    }
}
