#pragma once

#include <map>
#include <string>
#include <vector>

#include "../stdafx.h"

namespace libtorrent
{
    struct session_handle;
    class sha1_hash;
    struct torrent_status;
}

namespace pico
{
    class MainFrame : public CFrameWindowImpl<MainFrame>
    {
    public:
        DECLARE_FRAME_WND_CLASS(L"PicoTorrent", IDR_MAINFRAME)

        MainFrame(libtorrent::session_handle& session);

        void AddTorrent(libtorrent::torrent_status& status);
        void UpdateTorrent(libtorrent::torrent_status& status);

    private:
        LRESULT OnCopyData(HWND, PCOPYDATASTRUCT);
        LRESULT OnCreate(LPCREATESTRUCT);
        LRESULT OnDestroy();
        LRESULT OnTimer(UINT_PTR);
        LRESULT OnFileAddTorrent(UINT uNotifyCode, int nID, CWindow wndCtl);

        void ParseCommandLine(std::wstring cmdLine);
        void ShowAddTorrentDialog(std::vector<std::wstring>& files);

        BEGIN_MSG_MAP_EX(MainFrame)
            CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
            MSG_WM_COPYDATA(OnCopyData)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_TIMER(OnTimer);
            COMMAND_ID_HANDLER_EX(ID_FILE_ADD_TORRENT, OnFileAddTorrent)
        END_MSG_MAP()

        libtorrent::session_handle& session_;
        CListViewCtrl torrentsList_;
        std::map<libtorrent::sha1_hash, int> torrents_;
    };
}
