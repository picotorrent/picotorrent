#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    class Configuration;
    class Translator;

    class GeneralPage : public wxPanel
    {
    public:
        GeneralPage(wxWindow* parent, std::shared_ptr<Configuration> config, std::shared_ptr<Translator> translator);

        void ApplyConfiguration();
        bool ValidateConfiguration(wxString& error);

    private:
        wxChoice* m_language;
        wxCheckBox* m_autoStart;
        wxChoice* m_startPosition;
        wxCheckBox* m_showNotificationIcon;
        wxCheckBox* m_minimizeNotification;
        wxCheckBox* m_closeNotification;

        std::shared_ptr<Configuration> m_config;
    };
}
