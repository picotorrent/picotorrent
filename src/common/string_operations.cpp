#include <picotorrent/common/string_operations.hpp>

#include <windows.h>
#include <cctype>
#include <string>
#include <sstream>
#include <algorithm>

std::string picotorrent::common::join(const std::vector<std::string> &list, const std::string &delimiter)
{
    std::stringstream joined;
    for (size_t i = 0; i < list.size(); ++i)
    {
        if (i != 0)
        {
            joined << delimiter;
        }

        joined << trim(list[i]);
    }

    std::string s = joined.str();
    s.erase(std::remove(s.begin(), s.end(), '\0'), s.end());
    return s;
}

std::string picotorrent::common::to_string(const std::wstring &str)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), &result[0], size, NULL, NULL);
    return result;
}

std::wstring picotorrent::common::to_wstring(const std::string &str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring result(size, '\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}

std::string picotorrent::common::trim(const std::string &str)
{
    auto wsfront = std::find_if_not(str.begin(), str.end(), [](int c) {return std::isspace(c); });
    auto wsback = std::find_if_not(str.rbegin(), str.rend(), [](int c) {return std::isspace(c); }).base();
    return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}
