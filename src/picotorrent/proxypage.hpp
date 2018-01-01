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

    class ProxyPage : public wxPanel
    {
    public:
        ProxyPage(wxWindow* parent, std::shared_ptr<Configuration> config, std::shared_ptr<Translator> translator);

        void ApplyConfiguration();
        bool ValidateConfiguration(wxString& error);

    private:
        void UpdateUI();

        std::shared_ptr<Configuration> m_cfg;

        wxChoice* m_type;
        wxTextCtrl* m_host;
        wxTextCtrl* m_port;
        wxTextCtrl* m_username;
        wxTextCtrl* m_password;
        wxCheckBox* m_forceProxy;
        wxCheckBox* m_proxyHostnames;
        wxCheckBox* m_proxyPeers;
        wxCheckBox* m_proxyTrackers;
    };
}
