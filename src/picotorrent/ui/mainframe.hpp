#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <libtorrent/fwd.hpp>
#include <libtorrent/info_hash.hpp>

#include <map>
#include <vector>

class wxSplitterWindow;
class wxTaskBarIconEvent;

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

        void AddFilter(wxString const& name, std::function<bool(BitTorrent::TorrentHandle*)> const& filter);
        void HandleParams(std::vector<std::string> const& files, std::vector<std::string> const& magnets);

    private:
        wxMenuBar* CreateMainMenu();

        void AddTorrents(std::vector<libtorrent::add_torrent_params>& params);
        void CheckDiskSpace(std::vector<BitTorrent::TorrentHandle*> const& updatedTorrents);
        void OnClose(wxCloseEvent&);
        void OnFileAddMagnetLink(wxCommandEvent&);
        void OnFileAddTorrent(wxCommandEvent&);
        void OnFileCreateTorrent(wxCommandEvent&);
        void OnHelpAbout(wxCommandEvent&);
        void OnIconize(wxIconizeEvent&);
        void OnTaskBarLeftDown(wxTaskBarIconEvent&);
        void OnViewPreferences(wxCommandEvent&);
        void ParseTorrentFiles(std::vector<libtorrent::add_torrent_params>& params, std::vector<std::string> const& paths);
        void ShowTorrentContextMenu(wxCommandEvent&);

        wxSplitterWindow* m_splitter;

        StatusBar* m_statusBar;
        TaskBarIcon* m_taskBarIcon;
        TorrentDetailsView* m_torrentDetails;
        Models::TorrentListModel* m_torrentListModel;
        TorrentListView* m_torrentList;

        std::shared_ptr<Core::Environment> m_env;
        std::shared_ptr<Core::Database> m_db;
        std::shared_ptr<Core::Configuration> m_cfg;
        std::unique_ptr<IPC::Server> m_ipc;

        wxMenu* m_viewMenu;
        wxMenu* m_filtersMenu;
        wxMenuItem* m_menuItemFilters;
        wxMenuItem* m_menuItemDetailsPanel;
        wxMenuItem* m_menuItemStatusBar;

        std::map<size_t, std::function<bool(BitTorrent::TorrentHandle*)>> m_filters;

        BitTorrent::Session* m_session;
        std::map<libtorrent::info_hash_t, BitTorrent::TorrentHandle*> m_selection;
        int64_t m_torrentsCount;
    };
}
}
