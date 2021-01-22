#include "session_manager.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/write_resume_data.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace lt = libtorrent;
using pt::session_manager;

struct torrent_options
{
    bool is_initial;
};

static std::string to_str(lt::info_hash_t hash)
{
    std::stringstream ss;
    if (hash.has_v2()) { ss << hash.v2; }
    else { ss << hash.v1; }
    return ss.str();
}

static bool load_settings_pack(sqlite3* db, lt::settings_pack& settings)
{
    int res = sqlite3_exec(
        db,
        "SELECT name, value FROM settings WHERE name LIKE 'libtorrent.%' AND value IS NOT NULL",
        [](void* user, int, char** data, char**)
        {
            lt::settings_pack* sp = static_cast<lt::settings_pack*>(user);

            json j;

            try
            {
                j = json::parse(data[1]);
            }
            catch(const std::exception& e)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to parse setting '" << data[0] << "' as JSON: " << e.what() << ", value: " << data[1];
                return SQLITE_OK;
            }

            std::string name = data[0];
            name = name.substr(strlen("libtorrent.")); // trim 'libtorrent.'
            int setting = lt::setting_by_name(name);

            if (setting == -1)
            {
                BOOST_LOG_TRIVIAL(warning) << "Unknown libtorrent setting: " << name;
                return SQLITE_OK;
            }

            BOOST_LOG_TRIVIAL(debug) << "Setting '" << name << "' to " << j;

            if (setting >= lt::settings_pack::string_type_base
                && setting < lt::settings_pack::max_string_setting_internal)
            {
                sp->set_str(setting, j);
            }
            else if (setting >= lt::settings_pack::int_type_base
                && setting < lt::settings_pack::max_int_setting_internal)
            {
                sp->set_int(setting, j);
            }
            else if (setting >= lt::settings_pack::bool_type_base
                && setting < lt::settings_pack::max_bool_setting_internal)
            {
                sp->set_bool(setting, j);
            }
            else
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to determine type of setting for '" << name << "', " << setting;
            }

            return SQLITE_OK;
        },
        &settings,
        nullptr);

    return true;
}

std::shared_ptr<session_manager> session_manager::load(boost::asio::io_context& io, sqlite3* db)
{
    BOOST_LOG_TRIVIAL(info) << "Reading session params";

    lt::session_params params;

    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "SELECT data FROM session_params ORDER BY timestamp DESC LIMIT 1", -1, &stmt, nullptr);
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int len = sqlite3_column_bytes(stmt, 0);
            const char* buf = static_cast<const char*>(sqlite3_column_blob(stmt, 0));

            lt::error_code ec;
            lt::bdecode_node node = lt::bdecode(lt::span<const char>(buf, len), ec);

            if (ec)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to bdecode session params buffer: " << ec.message();
            }
            else
            {
                params = lt::read_session_params(node, lt::session::save_dht_state);
                BOOST_LOG_TRIVIAL(info) << "Loaded session params (" << len << " bytes)";
            }
        }

        sqlite3_finalize(stmt);
    }

    if (!load_settings_pack(db, params.settings))
    {
        BOOST_LOG_TRIVIAL(warning) << "Failed to read session settings: " << sqlite3_errmsg(db);
    }

    auto sm = std::shared_ptr<session_manager>(
        new session_manager(io, db, std::make_unique<lt::session>(params)));
    sm->load_torrents();

    return std::move(sm);
}

session_manager::session_manager(boost::asio::io_context& io, sqlite3* db, std::unique_ptr<lt::session> session)
    : m_io(io),
    m_db(db),
    m_session(std::move(session)),
    m_torrents(),
    m_timer(io)
{
    m_session->set_alert_notify(
        [this]()
        {
            boost::asio::post(
                m_io,
                std::bind(
                    &session_manager::read_alerts,
                    this));
        });

    boost::system::error_code ec;
    m_timer.expires_from_now(boost::posix_time::seconds(1), ec);
    m_timer.async_wait(
        std::bind(&session_manager::post_updates, this, std::placeholders::_1));
}

session_manager::~session_manager()
{
    BOOST_LOG_TRIVIAL(info) << "Saving session state";

    m_session->set_alert_notify([] {});
    m_timer.cancel();

    {
        std::vector<char> stateBuffer = lt::write_session_params_buf(
            m_session->session_state(),
            lt::session::save_dht_state);

        BOOST_LOG_TRIVIAL(debug) << "Saving session params (" << stateBuffer.size() << " bytes)";

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(m_db, "INSERT INTO session_params (data, timestamp) VALUES ($1, strftime('%s'));", -1, &stmt, nullptr);
        sqlite3_bind_blob(stmt, 1, stateBuffer.data(), static_cast<int>(stateBuffer.size()), SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            BOOST_LOG_TRIVIAL(warning) << "Failed to save session params data: " << sqlite3_errmsg(m_db);
        }

        sqlite3_finalize(stmt);
    }

    m_session->pause();

    // Save each torrents resume data
    int num_outstanding_resume = 0;
    int num_paused = 0;
    int num_failed = 0;

    auto temp = m_session->get_torrent_status([](lt::torrent_status const&) { return true; });

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

        ++num_outstanding_resume;
    }

    BOOST_LOG_TRIVIAL(info) << "Saving data for " << num_outstanding_resume << " torrent(s)";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(m_db, "UPDATE torrents SET queue_position = $1, resume_data = $2 WHERE info_hash = $3", -1, &stmt, nullptr);

    while (num_outstanding_resume > 0)
    {
        lt::alert const* tmp = m_session->wait_for_alert(lt::seconds(10));
        if (tmp == nullptr) { continue; }

        std::vector<lt::alert*> alerts;
        m_session->pop_alerts(&alerts);

        for (lt::alert* a : alerts)
        {
            lt::torrent_paused_alert* tp = lt::alert_cast<lt::torrent_paused_alert>(a);

            if (tp)
            {
                ++num_paused;
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++num_failed;
                --num_outstanding_resume;
                continue;
            }

            lt::save_resume_data_alert* rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --num_outstanding_resume;

            std::vector<char> buffer = lt::write_resume_data_buf(rd->params);
            std::string info_hash = to_str(rd->handle.info_hashes());

            // Store state
            sqlite3_bind_int(stmt, 1, static_cast<int>(rd->handle.status().queue_position));
            sqlite3_bind_blob(stmt, 2, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, info_hash.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to save resume data for torrent " << info_hash << ": " << sqlite3_errmsg(m_db);
            }

            sqlite3_reset(stmt);
        }
    }

    sqlite3_finalize(stmt);

    BOOST_LOG_TRIVIAL(info) << "Session state saved";
}

lt::info_hash_t session_manager::add_torrent(lt::add_torrent_params& params)
{
    params.userdata = lt::client_data_t(new torrent_options());

    m_session->async_add_torrent(params);

    return params.ti
        ? params.ti->info_hashes()
        : params.info_hashes;
}

void session_manager::for_each_torrent(std::function<bool(libtorrent::torrent_status const& ts)> const& iteree)
{
    for (auto const& item : m_torrents)
    {
        if (!iteree(item.second)) break;
    }
}

void session_manager::pause_torrent(lt::info_hash_t const& hash)
{
    m_torrents.at(hash).handle.pause();
}

void session_manager::reload_settings()
{
    lt::settings_pack sp;
    if (load_settings_pack(m_db, sp))
    {
        m_session->apply_settings(sp);
    }
}
void session_manager::remove_torrent(lt::info_hash_t const& hash, bool remove_files)
{
    m_session->remove_torrent(
        m_torrents.at(hash).handle);
}

std::shared_ptr<void> session_manager::subscribe(std::function<void(nlohmann::json&)> sub)
{
    auto ptr = std::make_shared<std::function<void(json&)>>(std::move(sub));
    m_subscribers.push_back(ptr);
    return ptr;
}

void session_manager::broadcast(json& j)
{
    size_t sz = m_subscribers.size();

    m_subscribers.erase(
        std::remove_if(
            m_subscribers.begin(),
            m_subscribers.end(),
            [](auto& ptr) { return ptr.expired(); }),
        m_subscribers.end());

    size_t pruned = sz - m_subscribers.size();

    if (pruned > 0)
    {
        BOOST_LOG_TRIVIAL(debug) << "Pruned " << pruned << " subscriber";
    }

    auto tmp = m_subscribers;

    for (auto pf : tmp)
    {
        if (auto s = pf.lock())
        {
            (*s)(j);
        }
    }
}

void session_manager::load_torrents()
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(m_db, "SELECT info_hash, resume_data, torrent_data FROM torrents ORDER BY queue_position ASC", -1, &stmt, nullptr);

    BOOST_LOG_TRIVIAL(info) << "Loading torrent state";

    while (true)
    {
        int res = sqlite3_step(stmt);

        if (res != SQLITE_ROW)
        {
            if (res == SQLITE_ERROR)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to step: " << sqlite3_errmsg(m_db);
            }

            break;
        }

        const char* info_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        int resume_len = sqlite3_column_bytes(stmt, 1);
        const char* resume_buf = static_cast<const char*>(sqlite3_column_blob(stmt, 1));

        int torrent_len = sqlite3_column_bytes(stmt, 2);
        const char* torrent_buf = static_cast<const char*>(sqlite3_column_blob(stmt, 2));

        lt::error_code ec;
        lt::add_torrent_params params;

        if (resume_len > 0)
        {
            params = lt::read_resume_data({ resume_buf, resume_len }, ec);
        }

        if (ec)
        {
            BOOST_LOG_TRIVIAL(warning) << "Failed to read resume data for torrent: " << ec.message();
            continue;
        }

        lt::bdecode_node node = lt::bdecode({ torrent_buf, torrent_len }, ec);

        if (ec)
        {
            BOOST_LOG_TRIVIAL(warning) << "Failed to parse torrent data for torrent: " << ec.message();
            continue;
        }

        auto extra = new torrent_options();
        extra->is_initial = true;

        params.ti = std::make_shared<lt::torrent_info>(node);
        params.userdata = lt::client_data_t(extra);

        m_session->async_add_torrent(params);
        m_initial_torrents++;
    }

    sqlite3_finalize(stmt);
}

void session_manager::read_alerts()
{
    std::vector<lt::alert*> alerts;
    m_session->pop_alerts(&alerts);

    for (auto const& alert : alerts)
    {
        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            lt::add_torrent_alert* ata = lt::alert_cast<lt::add_torrent_alert>(alert);
            lt::torrent_status ts = ata->handle.status();

            auto opts = std::unique_ptr<torrent_options>(ata->params.userdata.get<torrent_options>());

            if (opts == nullptr)
            {
                BOOST_LOG_TRIVIAL(error) << "No torrent_options for " << to_str(ts.info_hashes);
            }

            if (opts->is_initial)
            {
                m_initial_loaded++;
            }

            if (m_initial_torrents == m_initial_loaded)
            {
                BOOST_LOG_TRIVIAL(info) << "Torrent state loaded for " << m_initial_loaded << " torrent(s)";
            }

            if (ata->error)
            {
                BOOST_LOG_TRIVIAL(error) << "Error when adding torrent: " << ata->error;
                return;
            }

            if (!opts->is_initial)
            {
                BOOST_LOG_TRIVIAL(info) << "Torrent " << to_str(ata->handle.info_hashes()) << " added";
            }

            m_torrents.insert({ ts.info_hashes, ts });

            if (!opts->is_initial)
            {
                BOOST_LOG_TRIVIAL(info) << "Saving torrent " << to_str(ts.info_hashes) << " in database";

                lt::create_torrent ct(*ata->params.ti.get());
                std::vector<char> buffer;
                lt::bencode(std::back_inserter(buffer), ct.generate());

                // store in database
                sqlite3_stmt* stmt;
                sqlite3_prepare_v2(m_db, "INSERT INTO torrents (info_hash, queue_position, torrent_data) VALUES($1, $2, $3);", -1, &stmt, nullptr);
                sqlite3_bind_text(stmt, 1, to_str(ts.info_hashes).c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt, 2, static_cast<int>(ts.queue_position));
                sqlite3_bind_blob(stmt, 3, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);

                BOOST_LOG_TRIVIAL(debug) << "Stored torrent in database (" << buffer.size() << " bytes)";
            }

            json j;
            j["type"] = "torrent.added";
            j["info_hash"] = to_str(ts.info_hashes);
            j["name"] = ata->handle.status().name;
            j["progress"] = ts.progress;
            j["save_path"] = ts.save_path;
            j["size_wanted"] = ts.total_wanted;
            j["state"] = ts.state;
            j["muted"] = opts->is_initial;
            j["dl"] = ts.download_payload_rate;
            j["ul"] = ts.upload_payload_rate;

            broadcast(j);

            break;
        }
        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);

            json j;
            j["type"] = "torrent.updated";
            j["torrents"] = json::object();

            for (auto const& status : sua->status)
            {
                m_torrents.at(status.info_hashes) = status;

                json t;
                t["info_hash"] = to_str(status.info_hashes);
                t["name"] = status.name;
                t["progress"] = status.progress;
                t["save_path"] = status.save_path;
                t["size_wanted"] = status.total_wanted;
                t["state"] = status.state;
                t["dl"] = status.download_payload_rate;
                t["ul"] = status.upload_payload_rate;

                j["torrents"][to_str(status.info_hashes)] = t;
            }

            if (sua->status.size() > 0)
            {
                broadcast(j);
            }

            break;
        }
        case lt::torrent_removed_alert::alert_type:
        {
            lt::torrent_removed_alert* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);
            std::string info_hash = to_str(tra->info_hashes);

            BOOST_LOG_TRIVIAL(info) << "Removing torrent " << info_hash;

            m_torrents.erase(tra->info_hashes);

            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(m_db, "DELETE FROM torrents WHERE info_hash = $1", -1, &stmt, nullptr);
            sqlite3_bind_text(stmt, 1, info_hash.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);

            json j;
            j["type"] = "torrent.removed";
            j["info_hash"] = info_hash;

            broadcast(j);

            break;
        }
        }
    }
}

void session_manager::post_updates(boost::system::error_code ec)
{
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "error in timer: " << ec;
        return;
    }

    m_session->post_dht_stats();
    m_session->post_session_stats();
    m_session->post_torrent_updates();

    m_timer.expires_from_now(boost::posix_time::seconds(1), ec);
    m_timer.async_wait(
        std::bind(&session_manager::post_updates, this, std::placeholders::_1));
}
