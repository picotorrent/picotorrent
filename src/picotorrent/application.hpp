#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    struct ApplicationOptions;
    class MainFrame;

    class Application : public wxApp
    {
    public:
        Application();

        virtual bool OnCmdLineParsed(wxCmdLineParser&) wxOVERRIDE;
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser&) wxOVERRIDE;

    private:
        MainFrame* m_mainFrame;

        std::shared_ptr<ApplicationOptions> m_options;
    };
}
