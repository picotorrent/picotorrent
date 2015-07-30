#ifndef _PT_UI_TORRENTLISTVIEWCTRL_H
#define _PT_UI_TORRENTLISTVIEWCTRL_H

#include "../stdafx.h"
#include <libtorrent/torrent_handle.hpp>
#include <map>

class CTorrentListViewCtrl : public CWindowImpl<CTorrentListViewCtrl, WTL::CListViewCtrl>
{
    BEGIN_MSG_MAP(CTorrentListViewCtrl)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
    END_MSG_MAP()

public:
    void InsertTorrent(const libtorrent::torrent_status& status);

    void UpdateTorrent(const libtorrent::torrent_status& status);

protected:
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
    std::map<libtorrent::sha1_hash, int> items_;
};

#endif
