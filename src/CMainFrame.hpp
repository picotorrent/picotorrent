#pragma once

#include "stdafx.h"
#include "resources.h"

#include <map>
#include <memory>
#include <vector>

#define LT_SESSION_ALERT WM_USER+1

namespace libtorrent { class session; class sha1_hash; struct torrent_status; }

namespace UI
{
    class ListView;
    class StatusBar;
}

class CMainFrame : public WTL::CFrameWindowImpl<CMainFrame>
{
public:
    CMainFrame();
    ~CMainFrame();

private:
    void LoadState();
    void LoadTorrents();
    void SaveState();
    void SaveTorrents();

    void OnAlertNotify();
    void OnFileAddTorrent(UINT uNotifyCode, int nID, CWindow wndCtl);
    
    // Message handlers
    LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    LRESULT OnLVGetItemProgress(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnLVGetItemText(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    void OnTimerElapsed(UINT_PTR nIDEvent);

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_TIMER(OnTimerElapsed)

        // Command handlers
        COMMAND_ID_HANDLER_EX(ID_FILE_ADD_TORRENT, OnFileAddTorrent)

        // List view
        MESSAGE_HANDLER_EX(PT_LV_GETITEMPROGRESS, OnLVGetItemProgress)
        MESSAGE_HANDLER_EX(PT_LV_GETITEMTEXT, OnLVGetItemText)

        MESSAGE_HANDLER(LT_SESSION_ALERT, OnSessionAlert)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    END_MSG_MAP()

    std::shared_ptr<libtorrent::session> m_session;
    std::vector<libtorrent::sha1_hash> m_hashes;
    std::map<libtorrent::sha1_hash, libtorrent::torrent_status> m_torrents;
    std::shared_ptr<UI::ListView> m_torrentList;
    std::shared_ptr<UI::StatusBar> m_statusBar;
};
