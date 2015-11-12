#include <picotorrent/ui/notify_icon.hpp>

#include <picotorrent/logging/log.hpp>
#include <picotorrent/ui/resources.hpp>

#include <windows.h>
#include <shellapi.h>
#include <strsafe.h>

using picotorrent::ui::notify_icon;

notify_icon::notify_icon(HWND parent)
    : parent_(parent)
{
}

notify_icon::~notify_icon()
{
}

void notify_icon::add()
{
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = parent_;
    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
    nid.uFlags = NIF_ICON | NIF_TIP;
    nid.uID = WM_USER + 1;
    nid.uCallbackMessage = WM_USER + 2;

    // Set icon tooltip
    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), TEXT("PicoTorrent"));

    if (!Shell_NotifyIcon(NIM_ADD, &nid))
    {
        LOG(error) << "Failed to add notify icon: " << GetLastError();
    }
}

void notify_icon::remove()
{
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = parent_;
    nid.uID = WM_USER + 1;

    Shell_NotifyIcon(NIM_DELETE, &nid);
}
