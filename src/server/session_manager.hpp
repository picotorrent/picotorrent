#pragma once

#include <memory>

#include <boost/asio.hpp>
#include <libtorrent/fwd.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

namespace pt
{
    class session_manager
    {
    public:
        static std::shared_ptr<session_manager> load(boost::asio::io_context& io, sqlite3* db);

        ~session_manager();

        libtorrent::info_hash_t add_torrent(libtorrent::add_torrent_params& params);
        void for_each_torrent(std::function<bool(libtorrent::torrent_status const& ts)> const&);
        void pause_torrent(libtorrent::info_hash_t const& hash);
        void reload_settings();
        void remove_torrent(libtorrent::info_hash_t const& hash, bool remove_files = false);
        std::shared_ptr<void> subscribe(std::function<void(nlohmann::json&)>);

    private:
        session_manager(boost::asio::io_context& io, sqlite3* db, std::unique_ptr<libtorrent::session> session);

        void broadcast(nlohmann::json&);
        void load_torrents();
        void read_alerts();
        void post_updates(boost::system::error_code ec);

        boost::asio::io_context& m_io;
        boost::asio::deadline_timer m_timer;

        sqlite3* m_db;
        std::unique_ptr<libtorrent::session> m_session;
        size_t m_initial_torrents = 0;
        size_t m_initial_loaded = 0;
        std::map<libtorrent::info_hash_t, libtorrent::torrent_status> m_torrents;
        std::vector<std::weak_ptr<std::function<void(nlohmann::json&)>>> m_subscribers;
    };
}
