#pragma once

#include <picotorrent/export.hpp>
#include <string>

namespace picotorrent
{
namespace core
{
    DLL_EXPORT bool is_valid_torrent_file(const std::string& path);
}
}
