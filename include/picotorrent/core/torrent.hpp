#pragma once

#include <memory>
#include <string>

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

        int download_rate();
        std::shared_ptr<hash> info_hash();
        bool is_paused() const;
        bool is_valid();
        void move_storage(const std::string &path);
        std::string& name() const;
        void pause();
        float progress() const;
        int queue_position();
        void resume();
        std::string save_path() const;
        int64_t size();
        int upload_rate();

    protected:

    private:
        std::unique_ptr<libtorrent::torrent_status> status_;
    };
}
}
