#pragma once

#include <picotorrent/common.hpp>

namespace picotorrent
{
namespace core
{
namespace filesystem
{
    class path;
}

    DLL_EXPORT bool is_valid_torrent_file(const core::filesystem::path& path);
}
}
