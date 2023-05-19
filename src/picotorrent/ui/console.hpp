#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

#include "../core/configuration.hpp"

wxDECLARE_EVENT(ptEVT_FILTER_CHANGED, wxCommandEvent);

namespace pt::UI::Models { class TorrentListModel; }

namespace pt::UI
{
    class Console : public wxPanel
    {
    public:
        Console(wxWindow* parent, wxWindowID id, Models::TorrentListModel* model, bool isDarkMode);
        void SetText(std::string const& text);

    private:
        void CreateFilter(std::string const& filter);

        wxTextCtrl* m_input;
        Models::TorrentListModel* m_model;
    };
}
