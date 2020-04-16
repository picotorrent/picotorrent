#include "application.hpp"

#include <wx/persist.h>

#include "crashpadinitializer.hpp"
#include "persistencemanager.hpp"
#include "core/configuration.hpp"
#include "core/database.hpp"
#include "core/environment.hpp"
#include "ui/mainframe.hpp"
#include "ui/translator.hpp"

using pt::Application;

Application::Application()
    : wxApp(),
    m_mainFrame(nullptr)
{
    SetProcessDPIAware();
}

Application::~Application()
{
}

bool Application::OnInit()
{
    auto env = pt::Core::Environment::Create();

    if (env == nullptr)
    {
        // TODO: show some dialog
        return false;
    }

    pt::CrashpadInitializer::Initialize(env);

    auto db = std::make_shared<pt::Core::Database>(env);

    if (!db->Migrate())
    {
        return false;
    }

    auto cfg = std::make_shared<pt::Core::Configuration>(db);

    // Migrate old configuration to new database
    pt::Core::Configuration::Migrate(env, cfg);

    pt::UI::Translator& translator = pt::UI::Translator::GetInstance();
    translator.LoadEmbedded(GetModuleHandle(NULL));
    translator.SetLanguage(cfg->GetInt("language_id"));

    // Set up persistence manager
    m_persistence = std::make_unique<PersistenceManager>(db);
    wxPersistenceManager::Set(*m_persistence);

    m_mainFrame = new UI::MainFrame(env, db, cfg);
    m_mainFrame->Show();

    return true;
}
