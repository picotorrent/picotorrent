#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxPropertyGridManager;
class wxPropertyGridPage;

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
    class PreferencesAdvancedPage : public wxPanel
    {
    public:
        PreferencesAdvancedPage(wxWindow* parent, std::shared_ptr<pt::Core::Configuration> cfg);

        bool IsValid() { return true; };
        void Save();

    private:
        std::shared_ptr<pt::Core::Configuration> m_cfg;
        wxPropertyGridManager* m_grid;
        wxPropertyGridPage* m_page;
    };
}
}
}
