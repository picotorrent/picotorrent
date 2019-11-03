#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <filesystem>
#include <memory>

#include "loguru.hpp"

#include "core/configuration.hpp"
#include "core/database.hpp"
#include "core/environment.hpp"
#include "core/geoip/geoip.hpp"

#include "application.hpp"
#include "crashpad.hpp"
#include "mainwindow.hpp"
#include "translator.hpp"

namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    auto env = pt::Environment::create();

    if (env == nullptr)
    {
        // TODO: show some dialog
        return -1;
    }

    pt::CrashpadInitializer::init(env);

    pt::Application app(argc, argv);

    if (!app.isSingleInstance())
    {
        app.activateOtherInstance();
        return 0;
    }

    auto db = std::make_shared<pt::Database>(env);

    if (!db->migrate())
    {
        return -1;
    }

    auto cfg = std::make_shared<pt::Configuration>(db);

    // Migrate old configuration to new database
    pt::Configuration::migrate(env, cfg);

    pt::Translator& translator = pt::Translator::instance();
    translator.loadEmbedded(GetModuleHandle(NULL));
    translator.setLanguage(cfg->getInt("language_id"));

    pt::MainWindow wnd(env, db, cfg);

    switch (cfg->getInt("start_position"))
    {
    case pt::Configuration::WindowState::Hidden: // sys tray
        // If we don't show PicoTorrent in the notification area, show it
        // normally.
        if (!cfg->getBool("show_in_notification_area"))
        {
            wnd.showNormal();
        }
        break;
    case pt::Configuration::WindowState::Maximized:
        wnd.showMaximized();
        break;
    case pt::Configuration::WindowState::Minimized:
        wnd.showMinimized();
        break;

    case pt::Configuration::WindowState::Normal:
    default:
        wnd.showNormal();
        break;
    }

    wnd.handleCommandLine(app.arguments());

    return app.exec();
}
