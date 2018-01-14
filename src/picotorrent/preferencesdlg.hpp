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
    class Configuration;
    class ConnectionPage;
    class DownloadsPage;
    class Environment;
    class GeneralPage;
    class ProxyPage;
    struct SessionState;
    class TaskBarIcon;
    class Translator;

    class PreferencesDialog : public wxPropertySheetDialog
    {
    public:
        PreferencesDialog(
            wxWindow* parent,
            std::shared_ptr<Environment> env,
            std::shared_ptr<Configuration> cfg,
            std::shared_ptr<SessionState> sessionState,
            std::shared_ptr<TaskBarIcon> taskBarIcon,
            std::shared_ptr<Translator> translator);

    private:
        wxDECLARE_EVENT_TABLE();

        void OnOk(wxCommandEvent&);

        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Environment> m_env;
        std::shared_ptr<SessionState> m_state;
        std::shared_ptr<TaskBarIcon> m_taskBarIcon;

        ConnectionPage* m_connection;
        DownloadsPage* m_downloads;
        GeneralPage* m_general;
        ProxyPage* m_proxy;
    };
}
