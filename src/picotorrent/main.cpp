#include <QApplication>

#include <memory>

#include "configuration.hpp"
#include "database.hpp"
#include "environment.hpp"
#include "mainwindow.hpp"
#include "translator.hpp"

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    // Load environment and database
    pt::Translator& translator = pt::Translator::instance();
    translator.loadEmbedded(GetModuleHandle(NULL));
    translator.setLanguage(2052);

    auto env = std::make_shared<pt::Environment>();
    auto db = std::make_shared<pt::Database>("PicoTorrent.sqlite");
    auto cfg = std::make_shared<pt::Configuration>(db);

    if (!db->migrate())
    {
        return -1;
    }

    pt::MainWindow wnd(env, db, cfg);
    wnd.show();

    return app.exec();
}
