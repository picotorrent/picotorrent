#include <picotorrent/client/ui/notify_icon.hpp>

#include <picotorrent/core/logging/log.hpp>
#include <picotorrent/client/ui/resources.hpp>

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <strsafe.h>

using picotorrent::client::ui::notify_icon;

notify_icon::notify_icon(HWND parent)
    : parent_(parent)
{
    LoadIconMetric(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON), LIM_SMALL, &icon_);
}

notify_icon::~notify_icon()
{
    DestroyIcon(icon_);
}

void notify_icon::add()
{
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = parent_;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uID = WM_USER + 1;
    nid.uCallbackMessage = WM_NOTIFYICON;
    nid.hIcon = icon_;

    // Set icon tooltip
    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), TEXT("PicoTorrent"));

    if (!Shell_NotifyIcon(NIM_ADD, &nid))
    {
        LOG(error) << "Failed to add notify icon: " << GetLastError();
    }

    nid.uVersion = NOTIFYICON_VERSION_4;

    if (!Shell_NotifyIcon(NIM_SETVERSION, &nid))
    {
        LOG(error) << "Failed to set notify icon verion: " << GetLastError();
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

void notify_icon::show_balloon(const std::wstring &title, const std::wstring &text)
{
    NOTIFYICONDATA nid = { sizeof(NOTIFYICONDATA) };
    nid.hWnd = parent_;
    nid.uID = WM_USER + 1;
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = NIIF_USER;
    nid.hBalloonIcon = icon_;

    StringCchCopy(nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle), title.c_str());
    StringCchCopy(nid.szInfo, ARRAYSIZE(nid.szInfo), text.c_str());

    Shell_NotifyIcon(NIM_MODIFY, &nid);
}
