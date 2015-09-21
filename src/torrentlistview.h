#pragma once

#include "stdafx.h"

namespace pico
{
    class TorrentListView
    {
    public:
        TorrentListView(HWND hWndParent);
        void AddColumn(LPSTR text, int width, int format);
        void Create();
        HWND GetWindowHandle();
        void Resize(int width, int height);

    private:
        HWND hWnd_;
        HWND hWndParent_;
    };
}
