#include <QApplication>
#include <QtGlobal>

#include <memory>

#include "configuration.hpp"
#include "database.hpp"
#include "environment.hpp"
#include "mainwindow.hpp"
#include "translator.hpp"

static std::shared_ptr<pt::Database> db;

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    // Load environment and database
    pt::Translator& translator = pt::Translator::instance();
    translator.loadEmbedded(GetModuleHandle(NULL));
    translator.setLanguage(2052);

    auto env = std::make_shared<pt::Environment>();
    db = std::make_shared<pt::Database>("PicoTorrent.sqlite");
    auto cfg = std::make_shared<pt::Configuration>(db);

    if (!db->migrate())
    {
        return -1;
    }

    qInstallMessageHandler([](QtMsgType type, QMessageLogContext const& context, QString const& message)
    {
        auto stmt = db->statement("INSERT INTO log (category, file, function, line, version, message, timestamp) VALUES (?, ?, ?, ?, ?, ?, strftime('%s'));");
        stmt->bind(1, context.category);
        stmt->bind(2, context.file);
        stmt->bind(3, context.function);
        stmt->bind(4, context.line);
        stmt->bind(5, context.version);
        stmt->bind(6, message.toStdString());
        stmt->execute();
    });

    pt::MainWindow wnd(env, db, cfg);
    wnd.show();

    return app.exec();
}
