#pragma once

#include "Source.hpp"

namespace Sources
{
    class uTorrentSource : public Source
    {
    public:
        std::wstring GetName() { return L"uTorrent"; }
        std::vector<AddTorrentRequest> GetRequests();
        HWND GetWindowHandle(HINSTANCE hInstance, HWND hWndParent);

    private:
        static INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

        HWND m_hWnd;
    };
}
