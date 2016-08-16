#pragma once

#include "stdafx.h"
#include "resources.h"

#include <memory>

#define LT_SESSION_ALERT WM_USER+1

namespace libtorrent { class session; }

namespace UI
{
    class ListView;
    class StatusBar;
}

class CMainFrame : public WTL::CFrameWindowImpl<CMainFrame>
{
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
    LRESULT OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)

        // Command handlers
        COMMAND_ID_HANDLER_EX(ID_FILE_ADD_TORRENT, OnFileAddTorrent);

        MESSAGE_HANDLER(LT_SESSION_ALERT, OnSessionAlert)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    END_MSG_MAP()

    std::shared_ptr<libtorrent::session> m_session;
    std::shared_ptr<UI::ListView> m_torrentList;
    std::shared_ptr<UI::StatusBar> m_statusBar;
};
