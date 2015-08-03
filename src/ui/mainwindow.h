#ifndef _PT_UI_MAINWINDOW_H
#define _PT_UI_MAINWINDOW_H

#include "../stdafx.h"

#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <map>
#include <memory>

#include "torrentdetailsframe.h"
#include "torrentlistviewctrl.h"

class CMainWindow
    : public CFrameWindowImpl<CMainWindow>
{
public:
    CMainWindow();
    ~CMainWindow();

    DECLARE_FRAME_WND_CLASS_EX(NULL, IDR_MAINWINDOW, 0, -1);

    BEGIN_MSG_MAP(CMainWindow)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainWindow>)
        COMMAND_ID_HANDLER(ID_FILE_ADD_TORRENT, OnFileAddTorrent)
        COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
        COMMAND_ID_HANDLER(ID_APP_ABOUT, OnHelpAbout)
        
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_APP + 0x01, OnSessionAlert)
        MESSAGE_HANDLER(WM_TIMER, OnBackgroundTimer)

        NOTIFY_CODE_HANDLER(NM_DBLCLK, OnTorrentItemDoubleClick)
    END_MSG_MAP()

protected:
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

    LRESULT OnSessionAlert(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnBackgroundTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnFileAddTorrent(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnHelpAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnTorrentItemDoubleClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

    void OnSessionAlertNotify();

private:
    CTorrentListViewCtrl torrentList_;
    std::map<std::string, std::shared_ptr<CTorrentDetailsFrame>> torrentDetails_;
    std::unique_ptr<libtorrent::session> session_;
    std::vector<libtorrent::stats_metric> sessionMetrics_;
};

#endif
