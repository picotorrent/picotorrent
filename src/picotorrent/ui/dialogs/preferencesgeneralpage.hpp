#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

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
    class PreferencesGeneralPage : public wxPanel
    {
    public:
        PreferencesGeneralPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);
        virtual ~PreferencesGeneralPage();

        bool IsValid();
        void Save(bool* restartRequired);

    private:
        std::shared_ptr<Core::Configuration> m_cfg;

        wxChoice* m_language;
        wxChoice* m_theme;
        wxCheckBox* m_labelColor;
        wxCheckBox* m_skipAddTorrentDialog;
        wxCheckBox* m_autoStart;
        wxChoice* m_startPosition;
        wxCheckBox* m_showNotificationIcon;
        wxCheckBox* m_minimizeNotification;
        wxCheckBox* m_closeNotification;
    };
}
}
}
