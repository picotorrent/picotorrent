#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt
{
namespace UI
{
namespace Dialogs
{
    class AddTrackerDialog : public wxDialog
    {
    public:
        AddTrackerDialog(wxWindow* parent, wxWindowID id);
        virtual ~AddTrackerDialog();

        std::string GetUrl();

    private:
        wxTextCtrl* m_url;
    };
}
}
}
