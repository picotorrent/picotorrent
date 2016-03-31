#pragma once

#include <picotorrent/common.hpp>
#include <string>

namespace picotorrent
{
namespace core
{
    DLL_EXPORT bool is_valid_torrent_file(const std::string& path);
}
}
