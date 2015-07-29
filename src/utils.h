#ifndef _PT_UTILS_H
#define _PT_UTILS_H

#include <codecvt>
#include <locale>

namespace pt
{
    static std::wstring to_wstring(const std::string& str)
    {
        typedef std::codecvt_utf8<wchar_t> convert_type;
        std::wstring_convert<convert_type, wchar_t> converter;

        return converter.from_bytes(str);
    }

    static LPWSTR to_lpwstr(const std::string& str)
    {
        wchar_t* t = new wchar_t[str.size() + 1];
        mbstowcs(t, str.c_str(), str.size() + 1);
        return t;
    }
}

#endif
