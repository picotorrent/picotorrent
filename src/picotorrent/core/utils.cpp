#include "utils.hpp"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <sstream>
#include <iconv.h>
/*
#include <Windows.h>
#include <ShlObj.h>
#include <shlwapi.h>
*/
namespace fs = std::filesystem;

using pt::Utils;

void Utils::openAndSelect(fs::path path)
{
    std::wstringstream ss;
    ss << L"xdg-open " << path.parent_path() << L"\"";
    wxExecute(ss.str());
}

std::wstring Utils::toHumanFileSize(int64_t bytes)
{
    /*TCHAR buf[1024];
    StrFormatByteSize64(bytes, buf, 1024);
    return buf;*/
    return L"file size";

}

std::string Utils::toStdString(const std::wstring& input)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(input);
}

std::wstring Utils::toStdWString(const std::string& input)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(input);
}
