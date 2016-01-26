#pragma once

#include <memory>
#include <string>

#include <picotorrent/core/torrent_state.hpp>

namespace libtorrent
{
    struct torrent_status;
}

namespace picotorrent
{
namespace core
{
    class hash;
    class session;
    class torrent;

    typedef std::shared_ptr<torrent> torrent_ptr;

    class torrent
    {
        friend class session;

    public:
        torrent(const libtorrent::torrent_status &st);
        torrent(torrent&&) = default;
        torrent& operator=(torrent&&) = default;

        torrent(const torrent &that) = delete;

        int download_limit() const;
        int download_rate();
		int eta() const;
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
        uint64_t total_wanted();
        uint64_t total_wanted_done();
        int upload_limit() const;
        int upload_rate();

    private:
        void update(std::unique_ptr<libtorrent::torrent_status> status);
        void update_state();

        std::unique_ptr<libtorrent::torrent_status> status_;
        torrent_state state_;
    };
}
}
