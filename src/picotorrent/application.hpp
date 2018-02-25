#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

#include <wx/snglinst.h>

namespace pt
{
    struct ApplicationOptions;

    class Application : public wxApp
    {
    public:
        Application();

        virtual bool OnCmdLineParsed(wxCmdLineParser&) wxOVERRIDE;
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser&) wxOVERRIDE;

    private:
        std::unique_ptr<wxSingleInstanceChecker> m_singleInstance;
        std::shared_ptr<ApplicationOptions> m_options;
    };
}
