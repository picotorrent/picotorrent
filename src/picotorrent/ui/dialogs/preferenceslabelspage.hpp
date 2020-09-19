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
    class PreferencesLabelsPage : public wxPanel
    {
    public:
        PreferencesLabelsPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);

        bool IsValid();
        void Save();

    private:
        std::shared_ptr<Core::Configuration> m_cfg;
    };
}
}
}
