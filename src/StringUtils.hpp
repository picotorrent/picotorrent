#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <windows.h>

#define TS(val) ToString(val).c_str()
#define TWS(val) ToWideString(val).c_str()

static std::string ToString(const std::wstring &str)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), &result[0], size, NULL, NULL);
    return result;
}

static std::wstring ToWideString(const std::string& str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring result(size, '\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}

static std::string Trim(const std::string& str)
{
    auto wsfront = std::find_if_not(str.begin(), str.end(), [](int c) {return std::isspace(c); });
    auto wsback = std::find_if_not(str.rbegin(), str.rend(), [](int c) {return std::isspace(c); }).base();
    return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}
