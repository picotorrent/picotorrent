#include <picotorrent/client/ui/status_bar.hpp>

#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/scaler.hpp>

#include <commctrl.h>
#include <shlwapi.h>
#include <strsafe.h>

using picotorrent::client::ui::scaler;
using picotorrent::client::ui::status_bar;

status_bar::status_bar(HWND hParent)
{
    hWnd_ = CreateWindowEx(
        0,
        STATUSCLASSNAME,
        NULL,
        SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        hParent,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    RECT rc;
    GetClientRect(hWnd_, &rc);

    int parts[2] = {
        scaler::x(120),
        rc.right - rc.left
    };

    SendMessage(hWnd_, SB_SETPARTS, 2, (LPARAM)&parts);
}

HWND status_bar::handle()
{
    return hWnd_;
}

void status_bar::set_torrent_count(int count)
{
    TCHAR text[128];
    StringCchPrintf(text, ARRAYSIZE(text), to_wstring(TR("_torrent_s")).c_str(), count);
    SendMessage(handle(), SB_SETTEXT, 0, (LPARAM)text);
}

void status_bar::set_transfer_rates(int dl, int ul)
{
    TCHAR rates[128];
    StringCchPrintf(
        rates,
        ARRAYSIZE(rates),
        L"%s: %s, %s: %s",
        to_wstring(TR("dl")).c_str(),
        rate_to_string(dl).c_str(),
        to_wstring(TR("ul")).c_str(),
        rate_to_string(ul).c_str());

    SendMessage(handle(), SB_SETTEXT, 1, (LPARAM)rates);
}

std::wstring status_bar::rate_to_string(int rate)
{
    if (rate < 1024)
    {
        return L"-";
    }

    TCHAR t[128];
    StrFormatByteSize64(rate, t, ARRAYSIZE(t));

    return std::wstring(t) + L"/s";
}
