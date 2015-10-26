#pragma once

#include <string>

namespace picotorrent
{
namespace common
{
    std::string to_string(const std::wstring &str);
    std::wstring to_wstring(const std::string &str);
}
}
