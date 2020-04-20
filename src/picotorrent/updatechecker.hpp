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

    class UpdateChecker : private wxEvtHandler
    {
    public:
        UpdateChecker(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg);
        ~UpdateChecker();

        void Check(bool force = false);

    private:
        void ShowNoUpdateDialog();
        void ShowUpdateDialog(std::string const& version, std::string& url);

        wxWindow* m_parent;
        std::shared_ptr<Core::Configuration> m_cfg;
    };
}
