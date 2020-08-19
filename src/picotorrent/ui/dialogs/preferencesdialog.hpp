#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <wx/propdlg.h>

class wxBookCtrlEvent;

namespace pt
{
namespace Core
{
    class Configuration;
}
namespace UI
{
namespace Dialogs
{
    class PreferencesAdvancedPage;
    class PreferencesConnectionPage;
    class PreferencesDownloadsPage;
    class PreferencesGeneralPage;
    class PreferencesProxyPage;

    class PreferencesDialog : public wxPropertySheetDialog
    {
    public:
        PreferencesDialog(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);
        virtual ~PreferencesDialog();

    private:
        void OnOk(wxCommandEvent&);

        PreferencesGeneralPage* m_general;
        PreferencesDownloadsPage* m_downloads;
        PreferencesConnectionPage* m_connection;
        PreferencesProxyPage* m_proxy;
        PreferencesAdvancedPage* m_advanced;
    };
}
}
}
