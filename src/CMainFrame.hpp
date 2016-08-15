#pragma once

#include "stdafx.h"

#include <memory>

#define LT_SESSION_ALERT WM_USER+1

namespace libtorrent { class session; }

class CMainFrame : public WTL::CFrameWindowImpl<CMainFrame>
{
private:
    void LoadState();
    void LoadTorrents();
    void SaveState();
    void SaveTorrents();

    void OnAlertNotify();
    
    // Message handlers
    LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    LRESULT OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MESSAGE_HANDLER(LT_SESSION_ALERT, OnSessionAlert)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    END_MSG_MAP()

    std::shared_ptr<libtorrent::session> m_session;
};
