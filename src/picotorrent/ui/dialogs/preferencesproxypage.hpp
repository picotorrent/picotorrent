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
    class PreferencesProxyPage : public wxPanel
    {
    public:
        PreferencesProxyPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);

        bool IsValid();
        void Save();

    private:
        void UpdateUI();

        std::shared_ptr<Core::Configuration> m_cfg;

        wxChoice* m_type;
        wxTextCtrl* m_host;
        wxTextCtrl* m_port;
        wxTextCtrl* m_username;
        wxTextCtrl* m_password;
        wxCheckBox* m_proxyHostnames;
        wxCheckBox* m_proxyPeers;
        wxCheckBox* m_proxyTrackers;
    };
}
}
}
