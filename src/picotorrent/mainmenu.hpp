#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    class ApplicationUpdater;
    class Configuration;
    class Environment;
    struct SessionState;
    class TaskBarIcon;
    class Translator;
    class MainFrame;

    class MainMenu : public wxMenuBar
    {
    public:
        MainMenu(MainFrame const* mainFrame,
            std::shared_ptr<SessionState> state,
            std::shared_ptr<Configuration> cfg,
            std::shared_ptr<Environment> env,
            std::shared_ptr<ApplicationUpdater> updater,
            std::shared_ptr<TaskBarIcon> taskBarIcon,
            std::shared_ptr<Translator> translator);

        void SetDetailsToggle(bool value);

    private:
        enum
        {
            ptID_ADD_TORRENTS = wxID_HIGHEST + 1,
            ptID_ADD_MAGNET_LINK,
            ptID_VIEW_DETAILS_PANEL,
            ptID_VIEW_PREFERENCES,
            ptID_CHECK_FOR_UPDATES
        };

        wxDECLARE_EVENT_TABLE();

        void OnAbout(wxCommandEvent&);
        void OnAddMagnetLink(wxCommandEvent&);
        void OnAddTorrents(wxCommandEvent&);
        void OnCheckForUpdates(wxCommandEvent&);
        void OnExit(wxCommandEvent&);
        void OnViewDetailsPanel(wxCommandEvent&);
        void OnViewPreferences(wxCommandEvent&);

        std::shared_ptr<ApplicationUpdater> m_updater;
        std::shared_ptr<SessionState> m_state;
        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Environment> m_env;
        std::shared_ptr<TaskBarIcon> m_taskBarIcon;
        std::shared_ptr<Translator> m_trans;

        wxMenuItem* m_detailsToggle;
    };
}
