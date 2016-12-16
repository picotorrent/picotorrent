#pragma once

#include <windows.h>

namespace libtorrent { struct torrent_handle; }

namespace Controllers
{
    class TorrentDetailsController
    {
    public:
        TorrentDetailsController(HWND hWndParent, const libtorrent::torrent_handle& torrent);
        void Execute();

    private:
        const HWND m_hWndParent;
        const libtorrent::torrent_handle& m_torrent;
    };
}
