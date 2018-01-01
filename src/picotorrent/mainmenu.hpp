#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    class Configuration;
    class Environment;
    struct SessionState;
    class Translator;

    class MainMenu : public wxMenuBar
    {
    public:
        MainMenu(std::shared_ptr<SessionState> state,
            std::shared_ptr<pt::Configuration> cfg,
            std::shared_ptr<pt::Environment> env,
            std::shared_ptr<Translator> translator);

    private:
        enum
        {
            ptID_ADD_TORRENTS = wxID_HIGHEST + 1,
            ptID_ADD_MAGNET_LINK,
            ptID_VIEW_PREFERENCES
        };

        wxDECLARE_EVENT_TABLE();

        void OnAbout(wxCommandEvent&);
        void OnAddMagnetLink(wxCommandEvent&);
        void OnAddTorrents(wxCommandEvent&);
        void OnExit(wxCommandEvent&);
        void OnViewPreferences(wxCommandEvent&);

        std::shared_ptr<SessionState> m_state;
        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Environment> m_env;
        std::shared_ptr<Translator> m_trans;
    };
}
