#include "torrentlistviewctrl.h"

void CTorrentListViewCtrl::InsertTorrent(const libtorrent::torrent_handle& handle)
{
    int i = InsertItem(0, L"HELLO", 0);
}
