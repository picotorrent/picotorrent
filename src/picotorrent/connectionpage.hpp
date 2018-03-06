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

    class ConnectionPage : public wxPanel
    {
    public:
        ConnectionPage(wxWindow* parent, std::shared_ptr<Configuration> config, std::shared_ptr<Translator> translator);

        void ApplyConfiguration();
        bool ValidateConfiguration(wxString& error);

    private:
        wxWindow* m_parent;

        wxTextCtrl* m_listenInterfaces;
        wxCheckBox* m_incomingEncryption;
        wxCheckBox* m_outgoingEncryption;

        wxCheckBox* m_enableDht;
        wxCheckBox* m_enableLsd;
        wxCheckBox* m_enablePex;

        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Translator> m_translator;
    };
}
