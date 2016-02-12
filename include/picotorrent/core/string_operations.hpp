#pragma once

#include <picotorrent/common.hpp>
#include <string>

namespace picotorrent
{
namespace core
{
    DLL_EXPORT std::string to_string(const std::wstring &str);
    DLL_EXPORT std::wstring to_wstring(const std::string &str);
}
}
