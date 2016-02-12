#pragma once

#include <memory>
#include <picotorrent/common.hpp>
#include <string>
#include <vector>

namespace libtorrent
{
    class torrent_info;
}

namespace picotorrent
{
namespace core
{
namespace filesystem
{
    class path;
}

    class add_request;
    class torrent_info;

    typedef std::shared_ptr<torrent_info> torrent_info_ptr;

    class torrent_info
    {
        friend class add_request;

    public:
        DLL_EXPORT torrent_info(const std::vector<char> &buf);
        DLL_EXPORT torrent_info(const libtorrent::torrent_info &info);
        DLL_EXPORT ~torrent_info();

        DLL_EXPORT static torrent_info_ptr try_load(const filesystem::path &p);

        DLL_EXPORT std::string file_path(int index) const;
        DLL_EXPORT int64_t file_size(int index) const;
        DLL_EXPORT std::string name();
        DLL_EXPORT int num_files() const;
        DLL_EXPORT int64_t total_size();

    private:
        std::unique_ptr<libtorrent::torrent_info> info_;
    };
}
}
