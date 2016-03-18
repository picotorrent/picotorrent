#include <picotorrent/core/string_operations.hpp>

#include <windows.h>
#include <cctype>
#include <string>
#include <sstream>
#include <algorithm>

std::wstring picotorrent::core::join(const std::vector<std::wstring> &list, const std::wstring &delimiter)
{
    std::wstringstream joined;
    for (size_t i = 0; i < list.size(); ++i)
    {
        if (i != 0)
        {
            joined << delimiter;
        }

        joined << trim(list[i]);
    }

    std::wstring s = joined.str();
    s.erase(std::remove(s.begin(), s.end(), L'\0'), s.end());
    return s;
}

std::string picotorrent::core::to_string(const std::wstring &str)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), &result[0], size, NULL, NULL);
    return result;
}

std::wstring picotorrent::core::to_wstring(const std::string &str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring result(size, '\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}

std::wstring picotorrent::core::trim(const std::wstring &str)
{
    auto wsfront = std::find_if_not(str.begin(), str.end(), [](int c) {return std::isspace(c); });
    auto wsback = std::find_if_not(str.rbegin(), str.rend(), [](int c) {return std::isspace(c); }).base();
    return (wsback <= wsfront ? std::wstring() : std::wstring(wsfront, wsback));
}
