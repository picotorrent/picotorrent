#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <vector>

class wxDataViewCtrl;
class wxDataViewEvent;
class wxDirPickerCtrl;
class wxFileDirPickerEvent;

namespace libtorrent
{
    struct add_torrent_params;
}

namespace pt
{
    class FileStorageViewModel;
    class Translator;

    class AddTorrentDialog : public wxDialog
    {
    public:
        AddTorrentDialog(
            wxWindow* parent,
            std::shared_ptr<Translator> translator,
            std::vector<libtorrent::add_torrent_params>& params);

    private:
        enum
        {
            ptID_TORRENT_LIST = wxID_HIGHEST,
            ptID_SAVE_PATH,
            ptID_SEQUENTIAL_DOWNLOAD,
            ptID_START_TORRENT,
            ptID_TORRENT_FILE_LIST,
            ptID_FILE_LIST
        };

        void LoadTorrentInfo(int index);
        void OnFileContextMenu(wxDataViewEvent&);
        void OnSavePathChanged(wxFileDirPickerEvent&);
        void OnSetPriority(wxCommandEvent&);
        void OnSequentialDownloadChanged(wxCommandEvent&);
        void OnStartTorrentChanged(wxCommandEvent&);
        void OnTorrentChanged(wxCommandEvent&);

        wxDECLARE_EVENT_TABLE();

        wxChoice* m_torrents;
        wxStaticText* m_name;
        wxStaticText* m_size;
        wxStaticText* m_comment;
        wxStaticText* m_infoHash;
        wxDirPickerCtrl* m_savePath;
        wxCheckBox* m_sequentialMode;
        wxCheckBox* m_startTorrent;
        wxDataViewCtrl* m_filesView;
        FileStorageViewModel* m_filesViewModel;

        std::shared_ptr<Translator> m_trans;
        std::vector<libtorrent::add_torrent_params>& m_params;
    };
}
