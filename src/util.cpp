#include "util.h"

#include <shlwapi.h>
#define TORRENT_WINDOWS
#pragma warning(disable: 4005 4245 4267)
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/utf8.hpp>
#pragma warning(default: 4005 4245 4267)

namespace lt = libtorrent;
using namespace pico;

std::string Util::ToString(const std::wstring val)
{
    std::string result;
    libtorrent::wchar_utf8(val, result);
    return result;
}

std::wstring Util::ToWideString(const std::string val)
{
    std::wstring result;
    libtorrent::utf8_wchar(val, result);
    return result;
}

std::wstring Util::ToFileSize(int64_t size)
{
    TCHAR buf[100];
    StrFormatByteSizeEx(size, SFBS_FLAGS_TRUNCATE_UNDISPLAYED_DECIMAL_DIGITS, buf, sizeof(buf));
    return std::wstring(buf);
}

std::wstring Util::ToPriority(int prio)
{
    switch (prio)
    {
    case 0:
        return L"Do not download";
    case 1:
        return L"Normal";
    case 6:
        return L"High";
    case 7:
        return L"Maximum";
    default:
        return L"Unknown";
    }
}

std::wstring Util::ToSpeed(int64_t rate)
{
    if (rate < 1024)
    {
        return L"-";
    }

    return ToFileSize(rate) + L"/s";
}

std::wstring Util::ToState(int state)
{
    switch (state)
    {
    case lt::torrent_status::state_t::allocating:
        return L"Allocating";
    case lt::torrent_status::state_t::checking_files:
        return L"Checking files";
    case lt::torrent_status::state_t::checking_resume_data:
        return L"Checking resume data";
    case lt::torrent_status::state_t::downloading:
        return L"Downloading";
    case lt::torrent_status::state_t::downloading_metadata:
        return L"Downloading metadata";
    case lt::torrent_status::state_t::finished:
        return L"Finished";
    case lt::torrent_status::state_t::seeding:
        return L"Seeding";
    default:
        return L"Unknown";
    }
}
