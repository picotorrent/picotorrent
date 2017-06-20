#pragma once

#include <memory>
#include <string>
#include <vector>

#include <windows.h>

namespace libtorrent
{
    class torrent_info;
}

namespace Sources
{
    struct Source
    {
        struct AddTorrentRequest
        {
            std::vector<uint8_t> filePriorities;
            std::string savePath;
            std::shared_ptr<libtorrent::torrent_info> ti;
        };

        virtual ~Source() {}

        virtual HWND GetWindowHandle(HINSTANCE hInstance, HWND hWndParent) = 0;
        virtual std::vector<AddTorrentRequest> GetRequests() = 0;
        virtual std::wstring GetName() = 0;
    };
}
