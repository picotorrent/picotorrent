#pragma once

#include "stdafx.h"

namespace libtorrent
{
    class sha1_hash;
}

namespace pico
{
    class TorrentListView
    {
    public:
        TorrentListView(HWND hWndParent);
        void AddColumn(LPTSTR text, int width, int format);
        void Create();
        HWND GetWindowHandle();
        void Resize(int width, int height);
        void SetItemCount(int count);
        void Refresh();

    private:
        HWND hWnd_;
        HWND hWndParent_;
    };
}
