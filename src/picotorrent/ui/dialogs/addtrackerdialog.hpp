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

        int GetTier();
        std::string GetUrl();
        void SetTier(int tier);

    private:
        wxTextCtrl* m_url;
        wxTextCtrl* m_tier;
    };
}
}
}
