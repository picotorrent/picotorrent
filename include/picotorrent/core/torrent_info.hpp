#pragma once

#include <memory>
#include <string>
#include <vector>

namespace libtorrent
{
    class torrent_info;
}

namespace picotorrent
{
namespace filesystem
{
    class path;
}
namespace core
{
    class add_request;
    class torrent_info;

    typedef std::shared_ptr<torrent_info> torrent_info_ptr;

    class torrent_info
    {
        friend class add_request;

    public:
        torrent_info(const std::vector<char> &buf);
        torrent_info(const libtorrent::torrent_info &info);
        ~torrent_info();

        static torrent_info_ptr try_load(const filesystem::path &p);

        std::string file_path(int index) const;
        int64_t file_size(int index) const;
        std::string name();
        int num_files() const;
        int64_t total_size();

    private:
        std::unique_ptr<libtorrent::torrent_info> info_;
    };
}
}
