#pragma once

#include <picotorrent/common.hpp>
#include <string>
#include <vector>

namespace picotorrent
{
namespace core
{
    DLL_EXPORT std::wstring join(const std::vector<std::wstring> &list, const std::wstring &delimiter);
    DLL_EXPORT std::string to_string(const std::wstring &str);
    DLL_EXPORT std::wstring to_wstring(const std::string &str);
    DLL_EXPORT std::wstring trim(const std::wstring &str);
}
}
