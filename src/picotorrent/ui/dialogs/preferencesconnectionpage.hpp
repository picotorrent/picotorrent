#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxListView;

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
    class PreferencesConnectionPage : public wxPanel
    {
    public:
        PreferencesConnectionPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);
        virtual ~PreferencesConnectionPage();

        bool IsValid();
        void Save();
        void SetupNetworkAdapters();

    private:
        wxWindow* m_parent;

        wxListView* m_listenInterfaces;

        wxCheckBox* m_incomingEncryption;
        wxCheckBox* m_outgoingEncryption;

        wxCheckBox* m_enableDht;
        wxCheckBox* m_enableLsd;
        wxCheckBox* m_enablePex;

        std::shared_ptr<Core::Configuration> m_cfg;
        std::vector<int> m_removedListenInterfaces;
    };
}
}
}
