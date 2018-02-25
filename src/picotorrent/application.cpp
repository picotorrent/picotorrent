#include "application.hpp"

#include "applicationoptions.hpp"
#include "config.hpp"
#include "environment.hpp"
#include "mainframe.hpp"
#include "translator.hpp"

#include <wx/cmdline.h>
#include <wx/ipc.h>
#include <wx/taskbarbutton.h>

using pt::Application;

Application::Application()
    : m_options(nullptr),
    m_singleInstance(std::make_unique<wxSingleInstanceChecker>())
{
}

bool Application::OnCmdLineParsed(wxCmdLineParser& parser)
{
    m_options = std::make_shared<ApplicationOptions>();

    for (size_t i = 0; i < parser.GetParamCount(); i++)
    {
        wxString arg = parser.GetParam(i);

        if (arg.StartsWith("magnet:?xt"))
        {
            m_options->magnet_links.Add(arg);
        }
        else
        {
            m_options->files.Add(arg);
        }
    }

    return true;
}

bool Application::OnInit()
{
    if (!wxApp::OnInit())
    {
        return false;
    }

    if (m_singleInstance->IsAnotherRunning())
    {
        wxString json = ApplicationOptions::JsonEncode(m_options);

        wxClient client;
        auto conn = client.MakeConnection(
            "localhost",
            "PicoTorrent",
            "ApplicationOptions");

        conn->Execute(json);
        conn->Disconnect();

        return false;
    }

    auto env = std::make_shared<Environment>();
    auto cfg = Configuration::Load(env);
    auto translator = Translator::Load(GetModuleHandle(NULL), cfg);

    MainFrame* mainFrame = new MainFrame(
        cfg,
        env,
        translator);

    // Configured start-up position
    switch (cfg->StartPosition())
    {
    case Configuration::WindowState::Hidden:
        // Only valid if we have a notify icon
        if (cfg->UI()->ShowInNotificationArea())
        {
            mainFrame->MSWGetTaskBarButton()->Hide();
        }
        else
        {
            mainFrame->Show(true);
        }

        break;

    case Configuration::WindowState::Maximized:
        mainFrame->Show(true);
        mainFrame->Maximize();
        break;

    case Configuration::WindowState::Minimized:
        mainFrame->Iconize();
        mainFrame->Show(true);
        break;

    case Configuration::WindowState::Normal:
        mainFrame->Show(true);
        break;
    }

    mainFrame->HandleOptions(m_options);

    return true;
}

void Application::OnInitCmdLine(wxCmdLineParser& parser)
{
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_PARAM, NULL, NULL, "params", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },
        { wxCMD_LINE_NONE }
    };

    parser.SetDesc(cmdLineDesc);
    parser.SetSwitchChars("-");
}
