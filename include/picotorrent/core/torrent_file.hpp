#pragma once

#include <memory>
#include <vector>

namespace libtorrent
{
    class torrent_info;
}

namespace picotorrent
{
namespace core
{
    class add_request;
    class torrent_file;

    typedef std::shared_ptr<torrent_file> torrent_file_ptr;

    class torrent_file
    {
        friend class add_request;

    public:
        torrent_file(const std::vector<char> &buf);
        torrent_file(const libtorrent::torrent_info &info);
        ~torrent_file();

    private:
        std::unique_ptr<libtorrent::torrent_info> info_;
    };
}
}
