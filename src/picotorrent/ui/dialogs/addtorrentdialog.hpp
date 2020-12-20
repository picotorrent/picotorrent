#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/download_priority.hpp>
#include <libtorrent/fwd.hpp>
#include <libtorrent/info_hash.hpp>
#include <memory>
#include <set>
#include <vector>

class wxBitmapComboBox;
class wxButton;
class wxCheckBox;
class wxChoice;
class wxDataViewCtrl;
class wxDataViewEvent;
class wxDataViewItemArray;
class wxListView;
class wxSplitterWindow;

namespace pt::BitTorrent
{
    class Session;
}

namespace pt
{
namespace Core
{
    class Configuration;
    class Database;
}
namespace UI
{
namespace Models
{
    class FileStorageModel;
}
namespace Dialogs
{
    class AddTorrentDialog : public wxDialog
    {
    public:
        AddTorrentDialog(wxWindow* parent, wxWindowID id, libtorrent::add_torrent_params& params, std::shared_ptr<Core::Database> db, std::shared_ptr<Core::Configuration> cfg, std::shared_ptr<BitTorrent::Session> session);
        virtual ~AddTorrentDialog();

        libtorrent::add_torrent_params GetTorrentParams() { return m_params; };

    private:
        enum
        {
            ptID_TORRENTS_COMBO = wxID_HIGHEST + 1,
            ptID_LABEL_COMBO,
            ptID_SAVE_PATH_INPUT,
            ptID_SAVE_PATH_BROWSE,
            ptID_FILE_LIST,
            ptID_SEQUENTIAL_DOWNLOAD,
            ptID_START_TORRENT,
            ptID_SEED_MODE,
            ptID_TRACKERS_ADD,
            ptID_TRACKERS_REMOVE,
            ptID_OK,
            ptID_CANCEL,

            ptID_CONTEXT_MENU_DO_NOT_DOWNLOAD,
            ptID_CONTEXT_MENU_LOW,
            ptID_CONTEXT_MENU_NORMAL,
            ptID_CONTEXT_MENU_MAXIMUM,
        };

        void MetadataFound(std::shared_ptr<libtorrent::torrent_info> const& ti);

        wxString GetTorrentDisplayName(libtorrent::add_torrent_params const& params);
        wxString GetTorrentDisplaySize(libtorrent::add_torrent_params const& params);
        wxString GetTorrentDisplayInfoHash(libtorrent::add_torrent_params const& params);
        wxString GetTorrentDisplayComment(libtorrent::add_torrent_params const& params);

        void Load();
        void OnAddTracker(wxCommandEvent&);
        void OnCancel(wxCommandEvent&);
        void OnOk(wxCommandEvent&);
        void OnRemoveTracker(wxCommandEvent&);
        void ReloadTrackers();
        void SetFilePriorities(wxDataViewItemArray& items, libtorrent::download_priority_t prio);
        void ShowFileContextMenu(wxDataViewEvent&);

        wxSplitterWindow* m_splitter;
        wxChoice* m_torrents;
        wxStaticText* m_torrentName;
        wxStaticText* m_torrentSize;
        wxStaticText* m_torrentInfoHash;
        wxStaticText* m_torrentComment;
        wxBitmapComboBox* m_torrentLabel;
        wxComboBox* m_torrentSavePath;
        wxButton* m_torrentSavePathBrowse;
        wxDataViewCtrl* m_filesView;
        wxCheckBox* m_sequentialDownload;
        wxCheckBox* m_startTorrent;
        wxCheckBox* m_seedMode;
        wxListView* m_peers;
        wxListView* m_trackers;
        wxButton* m_addTracker;
        wxButton* m_removeTracker;

        Models::FileStorageModel* m_filesModel;

        libtorrent::add_torrent_params m_params;

        std::shared_ptr<Core::Configuration> m_cfg;
        std::shared_ptr<Core::Database> m_db;
        std::shared_ptr<BitTorrent::Session> m_session;
        std::set<libtorrent::info_hash_t> m_manualSavePath;
    };
}
}
}
