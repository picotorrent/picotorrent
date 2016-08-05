#pragma once

#include <picotorrent/export.hpp>
#include <string>
#include <vector>

namespace picotorrent
{
namespace common
{
    DLL_EXPORT std::string join(const std::vector<std::string> &list, const std::string &delimiter);
    DLL_EXPORT std::string to_string(const std::wstring &str);
    DLL_EXPORT std::wstring to_wstring(const std::string &str);
    DLL_EXPORT std::string trim(const std::string &str);
}
}
