#include "application.hpp"

#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>
#include <wx/cmdline.h>
#include <wx/ipc.h>
#include <wx/persist.h>
#include <wx/snglinst.h>
#include <wx/taskbarbutton.h>

#include "api/libpico_impl.hpp"
#include "crashpadinitializer.hpp"
#include "persistencemanager.hpp"
#include "core/configuration.hpp"
#include "core/database.hpp"
#include "core/environment.hpp"
#include "core/utils.hpp"
#include "ui/mainframe.hpp"
#include "ui/translator.hpp"

using json = nlohmann::json;
using pt::Application;

Application::Application()
    : wxApp(),
    m_singleInstance(std::make_unique<wxSingleInstanceChecker>("584c8e47-d8a5-4e52-9165-c0650a85723a"))
{
    SetProcessDPIAware();
}

Application::~Application()
{
    for (auto plugin : m_plugins)
    {
        delete plugin;
    }
}

bool Application::OnCmdLineParsed(wxCmdLineParser& parser)
{
    long waitForPid = -1;
    wxString save_path = "";

    if (parser.Found("wait-for-pid", &waitForPid))
    {
        m_options.pid = waitForPid;
    }

    m_options.silent = parser.Found("silent");

    if (parser.Found("save-path", &save_path))
    {
        m_options.save_path = Utils::toStdString(save_path.ToStdWstring());
    }

    for (size_t i = 0; i < parser.GetParamCount(); i++)
    {
        std::string arg = Utils::toStdString(parser.GetParam(i).ToStdWstring());

        if (arg.rfind("magnet:?xt", 0) == 0)
        {
            m_options.magnets.push_back(arg);
        }
        else
        {
            m_options.files.push_back(std::filesystem::absolute(arg).string());
        }
    }

    return true;
}

bool Application::OnInit()
{
    if (!wxApp::OnInit()) { return false; }

    if (m_options.pid > 0)
    {
        WaitForPreviousInstance(m_options.pid);
    }

    if (m_singleInstance->IsAnotherRunning())
    {
        ActivateOtherInstance();
        return false;
    }

    auto env = pt::Core::Environment::Create();
    pt::CrashpadInitializer::Initialize(env);

    auto db = std::make_shared<pt::Core::Database>(env);


    if (!db->Migrate())
    {
        wxMessageBox(
            "Failed to run database migrations. Please check log file.",
            "PicoTorrent",
            wxICON_ERROR);
        return false;
    }

    auto cfg = std::make_shared<pt::Core::Configuration>(db);

    // Load current locale
    pt::UI::Translator& translator = pt::UI::Translator::GetInstance();
    translator.LoadDatabase(env->GetCoreDbFilePath());
    translator.SetLocale(
        cfg->Get<std::string>("locale_name")
            .value_or(env->GetCurrentLocale()));
    
    // Load theme
    if (cfg->IsDarkMode())
    {
        wxApp::MSWEnableDarkMode();
    }

    // Load plugins
    for (auto& p : fs::directory_iterator(env->GetApplicationPath()))
    {
        if (p.path().extension() != ".dll") { continue; }

        auto const& filename = p.path().filename().string();

        if (filename.size() < 6) { continue; }
        if (filename.substr(0, 6) != "Plugin") { continue; }

        BOOST_LOG_TRIVIAL(info) << "Loading plugin from " << p.path();

        auto plugin = API::IPlugin::Load(p, env.get(), cfg.get());

        if (plugin != nullptr)
        {
            m_plugins.push_back(plugin);
        }
    }

    // Set up persistence manager
    m_persistence = std::make_unique<PersistenceManager>(db);
    wxPersistenceManager::Set(*m_persistence);

    auto mainFrame = new UI::MainFrame(env, db, cfg, m_options);

    std::for_each(
        m_plugins.begin(),
        m_plugins.end(),
        [mainFrame](auto plugin) { plugin->EmitEvent(libpico_event_mainwnd_created, mainFrame); });

    auto windowState = static_cast<pt::Core::Configuration::WindowState>(cfg->Get<int>("start_position").value());

    switch (windowState)
    {
    case pt::Core::Configuration::WindowState::Hidden:
        // Only valid if we have a notify icon
        if (cfg->Get<bool>("show_in_notification_area").value())
        {
            mainFrame->MSWGetTaskBarButton()->Hide();
        }
        else
        {
            mainFrame->Show(true);
        }

        break;

    case pt::Core::Configuration::WindowState::Maximized:
        mainFrame->Show(true);
        mainFrame->Maximize();
        break;

    case pt::Core::Configuration::WindowState::Minimized:
        mainFrame->Iconize();
        mainFrame->Show(true);
        break;

    case pt::Core::Configuration::WindowState::Normal:
        mainFrame->Show(true);
        break;
    }

    mainFrame->HandleParams(m_options);

    return true;
}

void Application::OnInitCmdLine(wxCmdLineParser& parser)
{
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_OPTION, NULL, "wait-for-pid",  NULL,   wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_SWITCH, NULL, "silent",        NULL,   wxCMD_LINE_VAL_NONE ,  wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_OPTION, NULL, "save-path",     NULL,   wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_PARAM,  NULL, NULL,           "params",wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },
        { wxCMD_LINE_NONE }
    };

    parser.SetDesc(cmdLineDesc);
    parser.SetSwitchChars("-");
}

void Application::ActivateOtherInstance()
{
    json j;
    j["files"] = m_options.files;
    j["magnet_links"] = m_options.magnets;
    j["silent"] = m_options.silent;
    j["save_path"] = m_options.save_path;

    wxClient client;
    auto conn = client.MakeConnection(
        "localhost",
        "PicoTorrent",
        "ApplicationOptions");

    if (conn)
    {
        conn->Execute(j.dump());
        conn->Disconnect();
    }
}

void Application::WaitForPreviousInstance(long pid)
{
    HANDLE hProc = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (hProc == NULL) { return; }
    WaitForSingleObject(hProc, 10000);
    CloseHandle(hProc);
}
