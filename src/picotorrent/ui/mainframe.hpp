#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <libtorrent/fwd.hpp>
#include <libtorrent/info_hash.hpp>

#include <map>
#include <unordered_set>
#include <vector>

class wxSplitterWindow;
class wxTaskBarIconEvent;

namespace pt::UI::Dialogs { class AddTorrentDialog; }

namespace pt
{
    class UpdateChecker;

namespace BitTorrent
{
    class Session;
    class TorrentHandle;
}
namespace Core
{
    class Configuration;
    class Database;
    class Environment;
}
namespace IPC
{
    class Server;
}
namespace UI
{
namespace Models
{
    class TorrentListModel;
}
    class Console;
    class StatusBar;
    class TaskBarIcon;
    class TorrentDetailsView;
    class TorrentListView;

    class MainFrame : public wxFrame
    {
    public:
        MainFrame(
            std::shared_ptr<Core::Environment> env,
            std::shared_ptr<Core::Database> db,
            std::shared_ptr<Core::Configuration> cfg);
        virtual ~MainFrame();

        void HandleParams(std::vector<std::string> const& files, std::vector<std::string> const& magnets);

    private:
        wxMenuBar* CreateMainMenu();

        void AddTorrents(std::vector<libtorrent::add_torrent_params>& params);
        void CheckDiskSpace(std::vector<BitTorrent::TorrentHandle*> const& updatedTorrents);
        void CreateFilterMenuItems();
        void CreateLabelMenuItems();
        void OnClose(wxCloseEvent&);
        void OnFileAddMagnetLink(wxCommandEvent&);
        void OnFileAddTorrent(wxCommandEvent&);
        void OnFileCreateTorrent(wxCommandEvent&);
        void OnHelpAbout(wxCommandEvent&);
        void OnViewHelp(wxCommandEvent&);
        void OnIconize(wxIconizeEvent&);
        void OnTaskBarLeftDown(wxTaskBarIconEvent&);
        void OnViewPreferences(wxCommandEvent&);
        void ParseTorrentFiles(std::vector<libtorrent::add_torrent_params>& params, std::vector<std::string> const& paths);
        void ShowTorrentContextMenu(wxCommandEvent&);
        void UpdateLabels();

        wxSplitterWindow* m_splitter;

        StatusBar* m_statusBar;
        TaskBarIcon* m_taskBarIcon;
        Console* m_console;
        TorrentDetailsView* m_torrentDetails;
        Models::TorrentListModel* m_torrentListModel;
        TorrentListView* m_torrentList;

        std::shared_ptr<BitTorrent::Session> m_session;
        std::shared_ptr<Core::Environment> m_env;
        std::shared_ptr<Core::Database> m_db;
        std::shared_ptr<Core::Configuration> m_cfg;
        std::unique_ptr<IPC::Server> m_ipc;

        wxMenu* m_viewMenu;
        wxMenu* m_filtersMenu;
        wxMenu* m_labelsMenu;
        wxMenuItem* m_menuItemLabels;
        wxMenuItem* m_menuItemFilters;
        wxMenuItem* m_menuItemDetailsPanel;
        wxMenuItem* m_menuItemConsoleInput;
        wxMenuItem* m_menuItemStatusBar;

        std::unordered_set<Dialogs::AddTorrentDialog*> m_addDialogs;
        std::map<libtorrent::info_hash_t, BitTorrent::TorrentHandle*> m_selection;
        int64_t m_torrentsCount;
    };
}
}
