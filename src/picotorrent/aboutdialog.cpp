#include "aboutdialog.hpp"

#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>

using pt::AboutDialog;

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    QVBoxLayout* layout = new QVBoxLayout();
    QIcon* ico = new QIcon(":res/app.ico");
    QLabel* l = new QLabel();
    l->setPixmap(ico->pixmap(64, 64));

    QLabel* t = new QLabel("PicoTorrent");
    t->setStyleSheet("QLabel { font-size: 18px; }");

    layout->addWidget(l);
    layout->addWidget(t);
    layout->setAlignment(l, Qt::AlignHCenter);
    layout->setAlignment(t, Qt::AlignHCenter);

    this->setLayout(layout);
    this->setWindowFlags(flags);
    this->setWindowTitle("About PicoTorrent");
}
