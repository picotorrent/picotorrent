#ifndef _PT_UTILS_H
#define _PT_UTILS_H

#include <codecvt>
#include <locale>
#include <stdint.h>
#include <windows.h>

#include <libtorrent/torrent_handle.hpp>

namespace lt = libtorrent;

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

    static std::string to_speed(uint64_t rate)
    {
        // If the rate is less than 1024 bytes/s
        // return "-" since it is quite uninteresting.
        if (rate < 1024)
        {
            return "-";
        }

        return to_file_size(rate) + "/s";
    }

    static std::string to_state_string(lt::torrent_status::state_t state)
    {
        switch (state)
        {
        case lt::torrent_status::state_t::allocating:
            return "Allocating";

        case lt::torrent_status::state_t::checking_files:
            return "Checking files";

        case lt::torrent_status::state_t::checking_resume_data:
            return "Checking resume data";

        case lt::torrent_status::state_t::downloading:
            return "Downloading";

        case lt::torrent_status::state_t::downloading_metadata:
            return "Downloading metadata";

        case lt::torrent_status::state_t::finished:
            return "Finished";

        case lt::torrent_status::state_t::seeding:
            return "Seeding";
        }

        return "Unknown";
    }
}

#endif
