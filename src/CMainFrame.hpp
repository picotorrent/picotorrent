#pragma once

#include "stdafx.h"

#include <memory>

#define LT_SESSION_ALERT WM_USER+1

namespace libtorrent { class session; }

class CMainFrame : public WTL::CFrameWindowImpl<CMainFrame>
{
private:
    void OnAlertNotify();
    
    // Message handlers
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    BEGIN_MSG_MAP(CMainFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(LT_SESSION_ALERT, OnSessionAlert);
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    END_MSG_MAP()

    std::shared_ptr<libtorrent::session> m_session;
};
