#pragma once

#include <functional>
#include <map>
#include <memory>
#include <thread>
#include <vector>

namespace libtorrent
{
    class session;
    class sha1_hash;
    class torrent_info;
}

namespace picotorrent
{
namespace core
{
    class add_request;
    class timer;
    class torrent;

    class session
    {
    public:
        session();
        ~session();

        void add_torrent(const add_request &add);

        void load();
        void unload();

        void remove_torrent(const std::shared_ptr<torrent> &torrent, bool remove_data = false);

        void on_torrent_added(const std::function<void(const std::shared_ptr<torrent>&)> &callback);
        void on_torrent_removed(const std::function<void(const std::shared_ptr<torrent>&)> &callback);
        void on_torrent_updated(const std::function<void(const std::shared_ptr<torrent>&)> &callback);

    private:
        typedef std::unique_ptr<libtorrent::session> session_ptr;
        typedef std::shared_ptr<torrent> torrent_ptr;
        typedef std::map<libtorrent::sha1_hash, torrent_ptr> torrent_map_t;

        void load_state();
        void load_torrents();
        void read_alerts();
        void remove_torrent_files(const torrent_ptr &torrent);
        void save_state();
        void save_torrent(const libtorrent::torrent_info &ti);
        void save_torrents();
        void timer_callback();

        std::unique_ptr<timer> timer_;
        torrent_map_t torrents_;
        session_ptr sess_;
        std::thread alert_thread_;
        bool is_running_;

        // Callbacks
        std::function<void(const torrent_ptr&)> torrent_added_cb_;
        std::function<void(const torrent_ptr&)> torrent_removed_cb_;
        std::function<void(const torrent_ptr&)> torrent_updated_cb_;
    };
}
}
