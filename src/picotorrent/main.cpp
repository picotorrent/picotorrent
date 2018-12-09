#include <QApplication>

#include <memory>

#include "configuration.hpp"
#include "database.hpp"
#include "environment.hpp"
#include "mainwindow.hpp"

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    // Load environment and database
    auto env = std::make_shared<pt::Environment>();
    auto db = std::make_shared<pt::Database>("PicoTorrent.sqlite");
    auto cfg = std::make_shared<pt::Configuration>(db);

    if (!db->migrate())
    {
        return -1;
    }

    pt::MainWindow wnd(env, cfg);
    wnd.show();

    return app.exec();
}
