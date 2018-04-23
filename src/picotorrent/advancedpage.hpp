#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxPropertyGrid;

namespace pt
{
    class Configuration;
    class Translator;

    class AdvancedPage : public wxPanel
    {
    public:
        AdvancedPage(wxWindow* parent, std::shared_ptr<Configuration> config, std::shared_ptr<Translator> translator);

        void ApplyConfiguration();
        bool ValidateConfiguration(wxString& error);

    private:
        wxWindow* m_parent;
        wxPropertyGrid* m_pg;

        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Translator> m_translator;
    };
}
