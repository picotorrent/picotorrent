#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <vector>

#include "applicationoptions.hpp"

class wxSingleInstanceChecker;

namespace pt
{
namespace API
{
    class IPlugin;
}

    class PersistenceManager;

    class Application : public wxApp
    {
    public:
        Application();
        virtual ~Application();

        virtual bool OnCmdLineParsed(wxCmdLineParser& parser) wxOVERRIDE;
        virtual bool OnInit() wxOVERRIDE;
        virtual void OnInitCmdLine(wxCmdLineParser&) wxOVERRIDE;

    private:
        void ActivateOtherInstance();
        void WaitForPreviousInstance(long pid);

        pt::CommandLineOptions m_options;
        std::vector<API::IPlugin*> m_plugins;
        std::unique_ptr<PersistenceManager> m_persistence;
        std::unique_ptr<wxSingleInstanceChecker> m_singleInstance;
    };
}
