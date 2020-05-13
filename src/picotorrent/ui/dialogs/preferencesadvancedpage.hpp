#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class wxPropertyGridManager;

namespace pt
{
namespace UI
{
namespace Dialogs
{
    class PreferencesAdvancedPage : public wxPanel
    {
    public:
        PreferencesAdvancedPage(wxWindow* parent);

    private:
        wxPropertyGridManager* m_grid;
    };
}
}
}
