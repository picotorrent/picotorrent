#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <thread>

class wxComboBox;

namespace pt
{
namespace UI
{
namespace Dialogs
{
    class CreateTorrentDialog : public wxDialog
    {
    public:
        explicit CreateTorrentDialog(wxWindow* parent, wxWindowID id);
        virtual ~CreateTorrentDialog();

    private:
        enum
        {
            ptID_BTN_BROWSE_FILE = wxID_HIGHEST + 1,
            ptID_BTN_BROWSE_DIR,
            ptID_BTN_CREATE_TORRENT,
            ptID_CHK_COMPAT_MODE
        };

        struct CreateTorrentParams;

        void GenerateTorrent(std::unique_ptr<CreateTorrentParams>);
        void OnBrowsePath(wxCommandEvent&);
        void OnCreateTorrent(wxCommandEvent&);

        wxTextCtrl* m_path;
        wxStaticText* m_numFiles;
        wxButton* m_selectFile;
        wxButton* m_selectDir;
        wxChoice* m_mode;
        wxCheckBox* m_private;
        wxTextCtrl* m_comment;
        wxTextCtrl* m_creator;
        wxTextCtrl* m_trackers;
        wxTextCtrl* m_urlSeeds;
        wxTextCtrl* m_status;
        wxGauge* m_progress;
        wxButton* m_create;

        std::thread m_worker;
    };
}
}
}
