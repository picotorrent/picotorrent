#pragma once

#include <memory>

#include "Source.hpp"

class IFileSystem;

namespace Sources
{
    class uTorrentSource : public Source
    {
    public:
        uTorrentSource(std::shared_ptr<IFileSystem> fileSystem);

        std::wstring GetName() { return L"uTorrent"; }
        std::vector<AddTorrentRequest> GetRequests();
        HWND GetWindowHandle(HINSTANCE hInstance, HWND hWndParent);

    private:
        static INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

        HWND m_hWnd;
        std::shared_ptr<IFileSystem> m_fileSystem;
    };
}
