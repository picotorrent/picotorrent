#include <QApplication>

#include "mainwindow.hpp"

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    pt::MainWindow wnd(app);
    wnd.show();

    return app.exec();
}
