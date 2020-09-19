#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxBookCtrlEvent;
class wxSimplebook;

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
    class PreferencesLabelsPage;
    class PreferencesProxyPage;

    class PreferencesDialog : public wxDialog
    {
    public:
        PreferencesDialog(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);
        virtual ~PreferencesDialog();

    private:
        enum
        {
            ptID_BTN_RESTORE_DEFAULTS = wxID_HIGHEST + 1
        };

        void OnOk(wxCommandEvent&);

        wxListBox* m_list;
        wxBoxSizer* m_mainSizer;
        wxSimplebook* m_book;

        PreferencesGeneralPage* m_general;
        PreferencesDownloadsPage* m_downloads;
        PreferencesLabelsPage* m_labels;
        PreferencesConnectionPage* m_connection;
        PreferencesProxyPage* m_proxy;
        PreferencesAdvancedPage* m_advanced;
    };
}
}
}
