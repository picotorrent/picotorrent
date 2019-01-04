#include <QApplication>
#include <QtGlobal>

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

    auto env = std::make_shared<pt::Environment>();
    auto db = std::make_shared<pt::Database>(env->getDatabaseFilePath().string());

    if (!db->migrate())
    {
        return -1;
    }

    auto cfg = std::make_shared<pt::Configuration>(db);

    pt::Translator& translator = pt::Translator::instance();
    translator.loadEmbedded(GetModuleHandle(NULL));
    translator.setLanguage(cfg->getInt("language_id"));

    pt::MainWindow wnd(env, db, cfg);
    wnd.show();

    return app.exec();
}
