#pragma once

namespace picotorrent
{
namespace filesystem
{
    class path;
}
namespace core
{
    bool is_valid_torrent_file(const filesystem::path& path);
}
}
