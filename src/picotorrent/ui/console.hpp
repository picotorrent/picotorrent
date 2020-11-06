#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt::UI::Models { class TorrentListModel; }

namespace pt::UI
{
    class Console : public wxPanel
    {
    public:
        Console(wxWindow* parent, wxWindowID id, Models::TorrentListModel* model);

    private:
        void CreateFilter(wxCommandEvent&);

        wxTextCtrl* m_input;
        Models::TorrentListModel* m_model;
    };
}
