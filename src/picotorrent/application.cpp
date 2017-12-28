#include "application.hpp"

#include "config.hpp"
#include "environment.hpp"
#include "mainframe.hpp"
#include "translator.hpp"

using pt::Application;

Application::Application()
{
    auto env = std::make_shared<Environment>();
    auto cfg = Configuration::Load(env);
    auto translator = Translator::Load(GetModuleHandle(NULL), cfg);

    m_mainFrame = new MainFrame(
        cfg,
        env,
        translator
    );
}

bool Application::OnInit()
{
    m_mainFrame->Show(true);
    return true;
}
