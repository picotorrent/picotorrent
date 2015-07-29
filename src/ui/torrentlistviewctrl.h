#ifndef _PT_UI_TORRENTLISTVIEWCTRL_H
#define _PT_UI_TORRENTLISTVIEWCTRL_H

#include "../stdafx.h"
#include <libtorrent/torrent_handle.hpp>

class CTorrentListViewCtrl : public CWindowImpl<CTorrentListViewCtrl, WTL::CListViewCtrl>
{
    BEGIN_MSG_MAP(CTorrentListViewCtrl)
    END_MSG_MAP()

public:
    void InsertTorrent(const libtorrent::torrent_handle& handle);
};

#endif
