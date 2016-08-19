#pragma once

#include "ListView.hpp"

#include <vector>

namespace libtorrent
{
    class file_storage;
}

namespace UI
{
    class TorrentFileListView : public ListView
    {
    public:
        TorrentFileListView(HWND hWnd,
            const libtorrent::file_storage& files,
            const std::vector<uint8_t>& prio);

    protected:
        std::wstring GetItemText(int columnId, int itemIndex);

    private:
        const libtorrent::file_storage& m_files;
        const std::vector<uint8_t>& m_prio;
    };
}
