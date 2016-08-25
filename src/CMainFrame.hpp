#pragma once

#include "stdafx.h"
#include "resources.h"

#include <map>
#include <memory>
#include <vector>

namespace libtorrent
{
    class session;
    struct stats_metric;
    class sha1_hash;
    struct torrent_handle;
    struct torrent_status;
}

namespace UI
{
    class StatusBar;
    class TorrentListView;
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
    void OnHelpAbout(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnViewPreferences(UINT uNotifyCode, int nID, CWindow wndCtl);
    
    // Message handlers
    LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    LRESULT OnRegisterNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnUnregisterNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnMoveTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnPauseTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnRemoveTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnResumeTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnShowTorrentDetails(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnTimerElapsed(UINT_PTR nIDEvent);

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_TIMER(OnTimerElapsed)

        // Command handlers
        MESSAGE_HANDLER_EX(PT_REGISTERNOTIFY, OnRegisterNotify)
        MESSAGE_HANDLER_EX(PT_UNREGISTERNOTIFY, OnUnregisterNotify)

        MESSAGE_HANDLER_EX(PT_MOVETORRENTS, OnMoveTorrents)
        MESSAGE_HANDLER_EX(PT_PAUSETORRENTS, OnPauseTorrents)
        MESSAGE_HANDLER_EX(PT_REMOVETORRENTS, OnRemoveTorrents)
        MESSAGE_HANDLER_EX(PT_RESUMETORRENTS, OnResumeTorrents)
        MESSAGE_HANDLER_EX(PT_SHOWTORRENTDETAILS, OnShowTorrentDetails)

        COMMAND_ID_HANDLER_EX(ID_FILE_ADD_TORRENT, OnFileAddTorrent)
        COMMAND_ID_HANDLER_EX(ID_HELP_ABOUT, OnHelpAbout)
        COMMAND_ID_HANDLER_EX(ID_VIEW_PREFERENCES, OnViewPreferences)

        MESSAGE_HANDLER(PT_ALERT, OnSessionAlert)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    END_MSG_MAP()

    std::vector<HWND> m_listeners;
    std::vector<libtorrent::stats_metric> m_metrics;
    std::vector<libtorrent::sha1_hash> m_muted_hashes;
    std::shared_ptr<libtorrent::session> m_session;
    std::map<libtorrent::sha1_hash, libtorrent::torrent_handle> m_torrents;
    std::shared_ptr<UI::StatusBar> m_statusBar;
    std::shared_ptr<UI::TorrentListView> m_torrentList;
};
