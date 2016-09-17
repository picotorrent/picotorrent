#pragma once

#include "stdafx.h"
#include "resources.h"

#include <map>
#include <memory>
#include <thread>
#include <vector>

#include <libtorrent/sha1_hash.hpp>
#include <picotorrent/api.hpp>

namespace libtorrent
{
    class session;
    struct stats_metric;
    struct torrent_handle;
    struct torrent_status;
}

namespace UI
{
    class NotifyIcon;
    class StatusBar;
    class Taskbar;
    class TorrentListView;
}

struct CommandLine;
class SleepManager;

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CMessageFilter, public IPicoTorrent
{
public:
    DECLARE_FRAME_WND_CLASS(TEXT("PicoTorrent/MainFrame"), IDR_MAINFRAME)

    CMainFrame();
    ~CMainFrame();

    // IPluginMembers
    void AddMenuItem(IMenuItem* menuItem);
    std::string GetCurrentVersion();
    void ShowDialog(TASKDIALOGCONFIG* tdcfg);

    void ActivateOtherInstance(LPTSTR lpstrCmdLine);
    bool IsSingleInstance();
    void HandleCommandLine(const CommandLine& cmdLine);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    void Show(int nCmdShow);

private:
    void OnFileAddTorrent(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnFileAddMagnetLink(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnFileExit(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnHelpAbout(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnViewPreferences(UINT uNotifyCode, int nID, CWindow wndCtl);
    
    // Message handlers
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    BOOL OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
    LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    LRESULT OnRegisterNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnUnregisterNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnFindMetadata(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnMoveTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnPauseTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnQueueTorrent(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnRemoveTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnRemoveTorrentsAccelerator(UINT uNotifyCode, int nID, CWindow wndCtl);
    LRESULT OnResumeTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnSelectAll(UINT uNotifyCode, int nID, CWindow wndCtl);
    LRESULT OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnShowTorrentDetails(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnInvoke(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnTaskbarButtonCreated(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnTimerElapsed(UINT_PTR nIDEvent);
    void OnUnhandledCommand(UINT uNotifyCode, int nID, CWindow wndCtl);

    BEGIN_MSG_MAP(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MSG_WM_COPYDATA(OnCopyData)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_TIMER(OnTimerElapsed)

        MESSAGE_HANDLER_EX(TaskbarButtonCreated, OnTaskbarButtonCreated)

        // Command handlers
        MESSAGE_HANDLER_EX(PT_ALERT, OnSessionAlert)
        MESSAGE_HANDLER_EX(PT_REGISTERNOTIFY, OnRegisterNotify)
        MESSAGE_HANDLER_EX(PT_UNREGISTERNOTIFY, OnUnregisterNotify)
        MESSAGE_HANDLER_EX(PT_FINDMETADATA, OnFindMetadata)
        MESSAGE_HANDLER_EX(PT_MOVETORRENTS, OnMoveTorrents)
        MESSAGE_HANDLER_EX(PT_PAUSETORRENTS, OnPauseTorrents)
        MESSAGE_HANDLER_EX(PT_QUEUETORRENT, OnQueueTorrent)
        MESSAGE_HANDLER_EX(PT_REMOVETORRENTS, OnRemoveTorrents)
        MESSAGE_HANDLER_EX(PT_RESUMETORRENTS, OnResumeTorrents)
        MESSAGE_HANDLER_EX(PT_SHOWTORRENTDETAILS, OnShowTorrentDetails)
        MESSAGE_HANDLER_EX(PT_NOTIFYICON, OnNotifyIcon)
        MESSAGE_HANDLER_EX(PT_INVOKE, OnInvoke)

        COMMAND_ID_HANDLER_EX(ID_FILE_ADD_TORRENT, OnFileAddTorrent)
        COMMAND_ID_HANDLER_EX(ID_FILE_ADD_MAGNET_LINK, OnFileAddMagnetLink)
        COMMAND_ID_HANDLER_EX(ID_FILE_EXIT, OnFileExit)
        COMMAND_ID_HANDLER_EX(ID_HELP_ABOUT, OnHelpAbout)
        COMMAND_ID_HANDLER_EX(ID_VIEW_PREFERENCES, OnViewPreferences)

        MSG_WM_COMMAND(OnUnhandledCommand)

        // Accelerators
        COMMAND_ID_HANDLER_EX(IDA_REMOVE_TORRENTS, OnRemoveTorrentsAccelerator)
        COMMAND_ID_HANDLER_EX(IDA_REMOVE_TORRENTS_DATA, OnRemoveTorrentsAccelerator)
        COMMAND_ID_HANDLER_EX(IDA_SELECT_ALL, OnSelectAll)

        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    END_MSG_MAP()

    static const UINT TaskbarButtonCreated;

    HANDLE m_mutex;
    bool m_singleInstance;

    std::map<int, IMenuItem*> m_extensionMenuItems;
    std::thread::id m_threadId;

    std::vector<HWND> m_listeners;
    std::vector<libtorrent::stats_metric> m_metrics;
    std::vector<libtorrent::sha1_hash> m_muted_hashes;
    std::vector<libtorrent::sha1_hash> m_find_metadata;
    std::shared_ptr<libtorrent::session> m_session;
    std::map<libtorrent::sha1_hash, libtorrent::torrent_handle> m_torrents;
    std::shared_ptr<UI::NotifyIcon> m_notifyIcon;
    std::shared_ptr<SleepManager> m_sleepManager;
    std::shared_ptr<UI::StatusBar> m_statusBar;
    std::shared_ptr<UI::Taskbar> m_taskbar;
    std::shared_ptr<UI::TorrentListView> m_torrentList;
};
