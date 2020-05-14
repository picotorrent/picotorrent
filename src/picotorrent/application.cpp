#include "application.hpp"

#include <loguru.hpp>
#include <wx/persist.h>

#include "api/libpico_impl.hpp"
#include "crashpadinitializer.hpp"
#include "persistencemanager.hpp"
#include "core/configuration.hpp"
#include "core/database.hpp"
#include "core/environment.hpp"
#include "ui/mainframe.hpp"
#include "ui/translator.hpp"

using pt::Application;

Application::Application()
    : wxApp()
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

bool Application::OnInit()
{
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

    // Migrate old configuration to new database
    pt::Core::Configuration::Migrate(env, cfg);

    pt::UI::Translator& translator = pt::UI::Translator::GetInstance();
    translator.LoadEmbedded(GetModuleHandle(NULL));
    translator.SetLanguage(cfg->GetInt("language_id"));

    // Load plugins
    for (auto& p : fs::directory_iterator(env->GetApplicationPath()))
    {
        if (p.path().extension() != ".dll") { continue; }

        auto const& filename = p.path().filename().string();

        if (filename.size() < 6) { continue; }
        if (filename.substr(0, 6) != "Plugin") { continue; }

        LOG_F(INFO, "Loading plugin from %s", p.path().string().c_str());

        auto plugin = API::IPlugin::Load(p, env.get(), cfg.get());

        if (plugin != nullptr)
        {
            m_plugins.push_back(plugin);
        }
    }

    // Set up persistence manager
    m_persistence = std::make_unique<PersistenceManager>(db);
    wxPersistenceManager::Set(*m_persistence);

    auto mainFrame = new UI::MainFrame(env, db, cfg);

    std::for_each(
        m_plugins.begin(),
        m_plugins.end(),
        [mainFrame](auto plugin) { plugin->EmitEvent(libpico_event_mainwnd_created, mainFrame); });

    mainFrame->Show();

    return true;
}
