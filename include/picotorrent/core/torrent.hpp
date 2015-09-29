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
        bool is_valid();
        std::string& name() const;
        void pause();
        float progress() const;
        int queue_position();
        void resume();
        int64_t size();
        int upload_rate();

    protected:

    private:
        std::unique_ptr<libtorrent::torrent_status> status_;
    };
}
}
