#pragma once

#include <windows.h>

#include <memory>
#include <string>
#include <vector>

namespace libtorrent
{
    class session;
}

namespace Controllers
{
    class AddMagnetLinkController
    {
    public:
        AddMagnetLinkController(HWND hWndOwner, const std::shared_ptr<libtorrent::session>& session);
        void Execute();
        void Execute(const std::vector<std::wstring>& magnetLinks);

    private:
        HWND m_hWndOwner;
        std::shared_ptr<libtorrent::session> m_session;
    };
}
