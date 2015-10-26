#include <picotorrent/common/string_operations.hpp>

#include <windows.h>

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
