#pragma once

#include "ListView.hpp"

#include <map>
#include <vector>

namespace libtorrent
{
    class sha1_hash;
    struct torrent_status;
}

namespace UI
{
    class TorrentListView : public ListView
    {
    public:
        TorrentListView(HWND hWnd,
            const std::vector<libtorrent::sha1_hash>& hashes,
            const std::map<libtorrent::sha1_hash, libtorrent::torrent_status>& torrents);

    protected:
        std::wstring GetItemText(int columnId, int itemIndex);

    private:
        const std::vector<libtorrent::sha1_hash>& m_hashes;
        const std::map<libtorrent::sha1_hash, libtorrent::torrent_status>& m_torrents;
    };
}
