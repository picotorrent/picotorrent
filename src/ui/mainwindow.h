#ifndef _PT_UI_MAINWINDOW_H
#define _PT_UI_MAINWINDOW_H

#include "../stdafx.h"

#include <libtorrent/session.hpp>
#include <map>
#include <memory>

#include "torrentdetailsframe.h"

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

        NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, OnLVGetDispInfo)
        NOTIFY_CODE_HANDLER(NM_DBLCLK, OnTorrentItemDoubleClick)
    END_MSG_MAP()

protected:
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

    LRESULT OnSessionAlert(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnFileAddTorrent(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnHelpAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnLVGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

    LRESULT OnTorrentItemDoubleClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

    void AlertDispatch(std::auto_ptr<libtorrent::alert> alert);

private:
    CListViewCtrl torrentList_;
    
    std::map<std::string, std::shared_ptr<CTorrentDetailsFrame>> torrentDetails_;

    std::unique_ptr<libtorrent::session> session_;
};

#endif
