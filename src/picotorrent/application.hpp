#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

#include <wx/snglinst.h>

namespace google_breakpad
{
    class ExceptionHandler;
}

namespace pt
{
    struct ApplicationOptions;

    class Application : public wxApp
    {
    public:
        Application();
        virtual ~Application();

        virtual bool OnCmdLineParsed(wxCmdLineParser&) wxOVERRIDE;
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser&) wxOVERRIDE;

    private:
        google_breakpad::ExceptionHandler* m_exceptionHandler;
        std::unique_ptr<wxSingleInstanceChecker> m_singleInstance;
        std::shared_ptr<ApplicationOptions> m_options;
    };
}
