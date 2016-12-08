#include "NotifyIcon.hpp"

#include <strsafe.h>

#include "../resources.h"

using UI::NotifyIcon;

NotifyIcon::NotifyIcon(HWND hWndParent)
{
    LoadIconMetric(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDR_MAINFRAME),
        LIM_SMALL,
        &m_hIcon);

    m_iconData = { 0 };
    m_iconData.cbSize = sizeof(NOTIFYICONDATA);
    m_iconData.hWnd = hWndParent;
    m_iconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_iconData.uID = 1337;
    m_iconData.uCallbackMessage = PT_NOTIFYICON;
    m_iconData.hIcon = m_hIcon;
    m_iconData.uVersion = NOTIFYICON_VERSION_4;
    m_iconData.dwInfoFlags = NIIF_USER;
    m_iconData.hBalloonIcon = m_hIcon;

    StringCchCopy(m_iconData.szTip, ARRAYSIZE(m_iconData.szTip), TEXT("PicoTorrent"));
}

NotifyIcon::~NotifyIcon()
{
    if (m_visible) { Hide(); }
    DestroyIcon(m_hIcon);
}

void NotifyIcon::Hide()
{
    Shell_NotifyIcon(NIM_DELETE, &m_iconData);
    m_visible = false;
}

void NotifyIcon::Show()
{
    Shell_NotifyIcon(NIM_ADD, &m_iconData);
    Shell_NotifyIcon(NIM_SETVERSION, &m_iconData);
    m_visible = true;
}


void NotifyIcon::ShowPopup(const std::wstring& title, const std::wstring& message)
{
    if (m_visible)
    {
        StringCchCopy(m_iconData.szInfoTitle, ARRAYSIZE(m_iconData.szInfoTitle), title.c_str());
        StringCchCopy(m_iconData.szInfo, ARRAYSIZE(m_iconData.szInfo), message.c_str());

        Shell_NotifyIcon(NIM_MODIFY, &m_iconData);
    }
}
