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

    private:
        wxMenuBar* CreateMainMenu();

        void AddTorrents(std::vector<libtorrent::add_torrent_params>& params);
        void OnFileAddMagnetLink(wxCommandEvent&);
        void OnFileAddTorrent(wxCommandEvent&);
        void OnHelpAbout(wxCommandEvent&);
        void ParseTorrentFiles(std::vector<libtorrent::add_torrent_params>& params, wxArrayString const& paths);
        void ShowTorrentContextMenu(wxCommandEvent&);

        wxSplitterWindow* m_splitter;

        StatusBar* m_statusBar;
        TaskBarIcon* m_taskBarIcon;
        TorrentDetailsView* m_torrentDetails;
        Models::TorrentListModel* m_torrentListModel;
        TorrentListView* m_torrentList;
        UpdateChecker* m_updateChecker;

        std::shared_ptr<Core::Environment> m_env;
        std::shared_ptr<Core::Database> m_db;
        std::shared_ptr<Core::Configuration> m_cfg;

        wxMenuItem* m_menuItemDetailsPanel;
        wxMenuItem* m_menuItemStatusBar;

        BitTorrent::Session* m_session;
        std::map<libtorrent::info_hash_t, BitTorrent::TorrentHandle*> m_selection;
        int64_t m_torrentsCount;
    };
}
}
