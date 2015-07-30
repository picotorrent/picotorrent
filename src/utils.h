#ifndef _PT_UTILS_H
#define _PT_UTILS_H

#include <codecvt>
#include <locale>
#include <stdint.h>
#include <windows.h>

namespace pt
{
    static std::wstring to_wstring(const std::string& str)
    {
        typedef std::codecvt_utf8<wchar_t> convert_type;
        std::wstring_convert<convert_type, wchar_t> converter;

        return converter.from_bytes(str);
    }

    static std::string to_string(const std::wstring& str)
    {
        typedef std::codecvt_utf8<wchar_t> convert_type;
        std::wstring_convert<convert_type, wchar_t> converter;

        return converter.to_bytes(str);
    }

    static LPWSTR to_lpwstr(const std::string& str)
    {
        wchar_t* t = new wchar_t[str.size() + 1];
        mbstowcs(t, str.c_str(), str.size() + 1);
        return t;
    }

    static std::string to_file_size(uint64_t size)
    {
        TCHAR buf[100];
        StrFormatByteSizeEx(size, SFBS_FLAGS_ROUND_TO_NEAREST_DISPLAYED_DIGIT, buf, sizeof(buf));
        return to_string(buf);
    }
}

#endif
