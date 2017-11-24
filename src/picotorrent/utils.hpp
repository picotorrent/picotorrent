#pragma once

#include <filesystem>
#include <string>

namespace libtorrent
{
    struct torrent_status;
}

namespace pt
{
    class Utils
    {
    public:
        static void OpenAndSelect(std::experimental::filesystem::v1::path path);
        static std::wstring ToHumanFileSize(int64_t bytes);
        static std::string ToReadableStatus(libtorrent::torrent_status const& ts);
    };
}
