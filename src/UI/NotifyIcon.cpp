#include "NotifyIcon.hpp"

#include <commctrl.h>
#include <shellapi.h>
#include <strsafe.h>

#include "../resources.h"

using UI::NotifyIcon;

NotifyIcon::NotifyIcon(HWND hWndParent)
    : m_hWndParent(hWndParent)
{
    LoadIconMetric(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDI_APPICON),
        LIM_SMALL,
        &m_hIcon);
}

NotifyIcon::~NotifyIcon()
{
    DestroyIcon(m_hIcon);
}

void NotifyIcon::Create()
{
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = m_hWndParent;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uID = WM_USER + 1;
    nid.uCallbackMessage = PT_NOTIFYICON;
    nid.hIcon = m_hIcon;

    // Set icon tooltip
    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), TEXT("PicoTorrent"));

    if (!Shell_NotifyIcon(NIM_ADD, &nid))
    {
        // LOG(error) << "Failed to add notify icon: " << GetLastError();
    }

    nid.uVersion = NOTIFYICON_VERSION_4;

    if (!Shell_NotifyIcon(NIM_SETVERSION, &nid))
    {
        // LOG(error) << "Failed to set notify icon verion: " << GetLastError();
    }
}

void NotifyIcon::Destroy()
{
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = m_hWndParent;
    nid.uID = WM_USER + 1;

    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void NotifyIcon::ShowPopup(const std::wstring& title, const std::wstring& message)
{
    NOTIFYICONDATA nid = { sizeof(NOTIFYICONDATA) };
    nid.hWnd = m_hWndParent;
    nid.uID = WM_USER + 1;
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = NIIF_USER;
    nid.hBalloonIcon = m_hIcon;

    StringCchCopy(nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle), title.c_str());
    StringCchCopy(nid.szInfo, ARRAYSIZE(nid.szInfo), message.c_str());

    Shell_NotifyIcon(NIM_MODIFY, &nid);
}
