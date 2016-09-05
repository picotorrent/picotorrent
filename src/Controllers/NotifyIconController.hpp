#pragma once

#include <memory>
#include <windows.h>

namespace libtorrent
{
    class session;
}

namespace Controllers
{
    class NotifyIconController
    {
    public:
        NotifyIconController(HWND hWnd, const std::shared_ptr<libtorrent::session>& session);
        ~NotifyIconController();

        void Execute(UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        HWND m_hWnd;
        std::shared_ptr<libtorrent::session> m_session;
    };
}
