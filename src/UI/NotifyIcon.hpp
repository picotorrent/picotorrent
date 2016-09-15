#pragma once

#include <string>

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>

namespace UI
{
    class NotifyIcon
    {
    public:
        NotifyIcon(HWND hWndParent);
        ~NotifyIcon();

        void Hide();
        void Show();
        void ShowPopup(const std::wstring& title, const std::wstring& message);

    private:
        NOTIFYICONDATA m_iconData;
        HICON m_hIcon;
        bool m_visible;
    };
}
