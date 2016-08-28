#pragma once

#include <string>
#include <windows.h>

namespace UI
{
    class NotifyIcon
    {
    public:
        NotifyIcon(HWND hWndParent);
        ~NotifyIcon();

        void Create();
        void Destroy();
        void ShowPopup(const std::wstring& title, const std::wstring& message);

    private:
        HWND m_hWndParent;
        HICON m_hIcon;
    };
}
