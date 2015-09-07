#pragma once

#include <map>
#include <mutex>
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
        void RemoveTorrent(libtorrent::sha1_hash& hash);
        void UpdateTorrent(libtorrent::torrent_status& status);

    private:
        void AddTorrentUnsafe(libtorrent::torrent_status& status);
        void RemoveTorrentUnsafe(libtorrent::sha1_hash& hash);
        void UpdateTorrentUnsafe(libtorrent::torrent_status& status);

        LRESULT OnContextMenu(CWindow, CPoint);
        LRESULT OnContextMenuCommand(WORD, WORD, HWND, BOOL&);
        LRESULT OnCopyData(HWND, PCOPYDATASTRUCT);
        LRESULT OnCreate(LPCREATESTRUCT);
        LRESULT OnDestroy();
        LRESULT OnTimer(UINT_PTR);
        LRESULT OnFileAddTorrent(UINT uNotifyCode, int nID, CWindow wndCtl);
        LRESULT OnFileExit(UINT uNotifyCode, int nID, CWindow wndCtl);

        void ParseCommandLine(std::wstring cmdLine);
        void ShowAddTorrentDialog(std::vector<std::wstring>& files);

        BEGIN_MSG_MAP_EX(MainFrame)
            CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
            MSG_WM_CONTEXTMENU(OnContextMenu)
            MSG_WM_COPYDATA(OnCopyData)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_TIMER(OnTimer)
            COMMAND_ID_HANDLER(ID_TORRENTCTX_PAUSE, OnContextMenuCommand)
            COMMAND_ID_HANDLER(ID_TORRENTCTX_RESUME, OnContextMenuCommand)
            COMMAND_ID_HANDLER(ID_TORRENTCTX_MANAGED, OnContextMenuCommand)
            COMMAND_ID_HANDLER(ID_TORRENTCTX_RECHECK, OnContextMenuCommand)
            COMMAND_ID_HANDLER(ID_TORRENTCTX_REMOVE, OnContextMenuCommand)
            COMMAND_ID_HANDLER_EX(ID_FILE_ADD_TORRENT, OnFileAddTorrent)
            COMMAND_ID_HANDLER_EX(ID_FILE_EXIT, OnFileExit)
        END_MSG_MAP()

        libtorrent::session_handle& session_;
        std::map<libtorrent::sha1_hash, int> torrents_;
        std::mutex mtx_;;
        CListViewCtrl torrentsList_;
    };
}
