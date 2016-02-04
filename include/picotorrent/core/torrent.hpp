#pragma once

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <picotorrent/common/signals/signal.hpp>
#include <picotorrent/core/torrent_state.hpp>

namespace libtorrent
{
    struct peer_info;
    struct scrape_reply_alert;
    struct torrent_status;
    struct tracker_reply_alert;
}

namespace picotorrent
{
namespace core
{
    class hash;
    class peer;
    class session;
    class torrent;
    class torrent_info;
    class tracker;
    class tracker_status;

    typedef std::shared_ptr<torrent> torrent_ptr;

    class torrent
    {
        friend class session;

    public:
        enum priority_t
        {
            do_not_download = 0,
            normal = 4,
            high = 6,
            maximum = 7
        };

        torrent(const libtorrent::torrent_status &st);
        torrent(torrent&&) = default;
        torrent& operator=(torrent&&) = default;

        torrent(const torrent &that) = delete;
        ~torrent();

        int download_limit() const;
        int download_rate();
		int eta() const;
        std::vector<int> file_priorities() const;
        void file_priority(int file_index, int priority);
        void file_progress(std::vector<int64_t> &progress, int flags = 0) const;
        std::vector<peer> get_peers();
        std::vector<tracker> get_trackers();
        tracker_status& get_tracker_status(const std::string &url);
        bool has_error() const;
        std::shared_ptr<hash> info_hash();
        bool is_checking() const;
        bool is_forced() const;
        bool is_paused() const;
        bool is_queued() const;
        bool is_seeding() const;
        bool is_valid();
        int max_connections() const;
        int max_uploads() const;
        void move_storage(const std::string &path);
        std::string& name() const;
        void pause();
        float progress() const;
        int queue_position();
        void resume(bool force);
        std::string save_path() const;
        void set_download_limit(int limit);
        void set_max_connections(int limit);
        void set_max_uploads(int limit);
        void set_sequential_download(bool val);
        void set_upload_limit(int limit);
        bool sequential_download() const;
        int64_t size();
        torrent_state state();
        std::shared_ptr<const torrent_info> torrent_info() const;
        uint64_t total_wanted();
        uint64_t total_wanted_done();
        int upload_limit() const;
        int upload_rate();

        // Signals
        common::signals::signal_connector<void, void>& on_updated();

        void register_updated_callback(const std::function<void()> &callback);
        void unregister_updated_callback(const std::function<void()> &callback);

    private:
        void handle(const libtorrent::scrape_reply_alert &alert);
        void handle(const libtorrent::tracker_reply_alert &alert);
        void update(std::unique_ptr<libtorrent::torrent_status> status);
        void update_state();

        std::map<std::string, tracker_status> tracker_status_;
        std::unique_ptr<libtorrent::torrent_status> status_;
        torrent_state state_;

        common::signals::signal<void, void> updated_signal_;
    };
}
}
