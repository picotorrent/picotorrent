#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt::Core { class Configuration; }

namespace pt::UI::Dialogs
{
    class PreferencesRssPage : public wxPanel
    {
    public:
        PreferencesRssPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);
        virtual ~PreferencesRssPage();

        bool IsValid();
        void Save();

    private:
        std::shared_ptr<Core::Configuration> m_cfg;
    };
}
