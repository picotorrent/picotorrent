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
    class Configuration;
    class Translator;

    class PreferencesConnectionPage : public wxPanel
    {
    public:
        PreferencesConnectionPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);

        bool IsValid();
        void Save();
        void SetupNetworkAdapters();

    private:
        wxWindow* m_parent;

        wxChoice* m_listenInterfaces;
        wxTextCtrl* m_listenPort;
        wxCheckBox* m_incomingEncryption;
        wxCheckBox* m_outgoingEncryption;

        wxCheckBox* m_enableDht;
        wxCheckBox* m_enableLsd;
        wxCheckBox* m_enablePex;

        std::shared_ptr<Core::Configuration> m_cfg;
    };
}
}
}
