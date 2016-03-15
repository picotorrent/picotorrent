#pragma once

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <picotorrent/common.hpp>
#include <picotorrent/core/signals/signal.hpp>
#include <picotorrent/core/torrent_state.hpp>
#include <picotorrent/core/tracker_status.hpp>

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

        DLL_EXPORT torrent(const libtorrent::torrent_status &st);
        DLL_EXPORT torrent(torrent&&) = default;
        DLL_EXPORT torrent& operator=(torrent&&) = default;

        DLL_EXPORT torrent(const torrent &that) = delete;
        DLL_EXPORT ~torrent();

        DLL_EXPORT void add_tracker(const std::string &url);
        DLL_EXPORT void clear_error();
        DLL_EXPORT int download_limit() const;
        DLL_EXPORT int download_rate();
        DLL_EXPORT std::string error_message() const;
		DLL_EXPORT int eta() const;
        DLL_EXPORT std::vector<int> file_priorities() const;
        DLL_EXPORT void file_priority(int file_index, int priority);
        DLL_EXPORT void file_progress(std::vector<int64_t> &progress, int flags = 0) const;
        DLL_EXPORT std::vector<peer> get_peers();
        DLL_EXPORT std::vector<tracker> get_trackers();
        DLL_EXPORT tracker_status& get_tracker_status(const std::string &url);
        DLL_EXPORT bool has_error() const;
        DLL_EXPORT std::shared_ptr<hash> info_hash();
        DLL_EXPORT bool is_checking() const;
        DLL_EXPORT bool is_forced() const;
        DLL_EXPORT bool is_paused() const;
        DLL_EXPORT bool is_queued() const;
        DLL_EXPORT bool is_seeding() const;
        DLL_EXPORT bool is_valid();
        DLL_EXPORT int max_connections() const;
        DLL_EXPORT int max_uploads() const;
        DLL_EXPORT void move_storage(const std::string &path);
        DLL_EXPORT std::string& name() const;
        DLL_EXPORT void pause();
        DLL_EXPORT float progress() const;
        DLL_EXPORT int queue_position();
        DLL_EXPORT void queue_up();
        DLL_EXPORT void queue_down();
        DLL_EXPORT void queue_top();
        DLL_EXPORT void queue_bottom();
        DLL_EXPORT void remove_trackers(const std::vector<std::string> &trackers);
        DLL_EXPORT void resume(bool force);
        DLL_EXPORT std::string save_path() const;
        DLL_EXPORT void set_download_limit(int limit);
        DLL_EXPORT void set_max_connections(int limit);
        DLL_EXPORT void set_max_uploads(int limit);
        DLL_EXPORT void set_sequential_download(bool val);
        DLL_EXPORT void set_upload_limit(int limit);
        DLL_EXPORT bool sequential_download() const;
        DLL_EXPORT int64_t size();
        DLL_EXPORT torrent_state state();
        DLL_EXPORT std::shared_ptr<const torrent_info> torrent_info() const;
        DLL_EXPORT uint64_t total_wanted();
        DLL_EXPORT uint64_t total_wanted_done();
        DLL_EXPORT int upload_limit() const;
        DLL_EXPORT int upload_rate();

        // Signals
        DLL_EXPORT signals::signal_connector<void, void>& on_updated();

    private:
        void handle(const libtorrent::scrape_reply_alert &alert);
        void handle(const libtorrent::tracker_reply_alert &alert);
        void update(std::unique_ptr<libtorrent::torrent_status> status);
        void update_state();

        std::map<std::string, tracker_status> tracker_status_;
        std::shared_ptr<libtorrent::torrent_status> status_;
        torrent_state state_;

        signals::signal<void, void> updated_signal_;
    };
}
}
