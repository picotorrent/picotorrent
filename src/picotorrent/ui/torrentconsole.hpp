#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt::UI
{
    class TorrentConsole : public wxPanel
    {
    public:
        TorrentConsole(wxWindow* parent, wxWindowID id);

    private:
        void CreateFilter(wxCommandEvent&);

        wxTextCtrl* m_input;
    };
}
