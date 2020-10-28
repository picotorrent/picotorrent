#include "utils.hpp"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#include <shlwapi.h>
#endif

namespace fs = std::filesystem;

using pt::Utils;

void Utils::openAndSelect(fs::path path)
{
    std::wstringstream ss;
    ss << L"explorer.exe /select,\"" << path.native() << L"\"";
    wxExecute(ss.str());
}

std::wstring Utils::toHumanFileSize(int64_t bytes)
{
#ifdef _WIN32
    TCHAR buf[1024];
    StrFormatByteSize64(bytes, buf, 1024);
    return buf;
#else
    return L"-";
#endif
}

std::string Utils::toStdString(std::wstring const& input)
{
#ifdef _WIN32
    int size = WideCharToMultiByte(CP_UTF8, 0, &input[0], (int)input.size(), NULL, 0, NULL, NULL);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &input[0], (int)input.size(), &result[0], size, NULL, NULL);
#else
    std::string result;
#endif
    return result;
}

std::wstring Utils::toStdWString(std::string const& input)
{
#ifdef _WIN32
    int size = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, NULL, 0);
    std::wstring result(size, '\0');
    MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, &result[0], size);
#else
    std::wstring result;
#endif
    return result;
}
