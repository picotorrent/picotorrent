#include "aboutdialog.hpp"

#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>

using pt::AboutDialog;

AboutDialog::AboutDialog(QWidget* parent, std::string version)
    : QDialog(parent),
    m_version(version)
{
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    // All constants defined here, later replaced by translations
    QString info_str = "Built with love, using Boost, Rasterbar-libtorrent, Openssl and Qt";
    QString heading = QString::fromStdString("Picotorrent v" + m_version);
    QString dev_heading = "Developers";
    QString window_title = "About Picotorrent";
    QList<QString> developers;
    developers.push_back(QString("Viktor Elofsson"));

    // All widgets defined
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addSpacing(3);

    QIcon* ico = new QIcon(":res/app.ico");
    QLabel* l = new QLabel();
    l->setPixmap(ico->pixmap(64, 64));

    QLabel* t = new QLabel(heading);
    t->setStyleSheet("QLabel { font-size: 18px; }");

    QString c = QChar(0xA9);
    c.append(" 2015-2019");
    QLabel* copyright = new QLabel(c);

    QLabel* info = new QLabel(info_str);

    QString li = R"s(<a href="https://picotorrent.org">https://picotorrent.org</a>)s";
    QLabel* link = new QLabel(li);

    QLabel* dev = new QLabel(dev_heading);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    // Adding widgets
    layout->addWidget(l);
    layout->addWidget(t);
    layout->addWidget(copyright);
    layout->addWidget(info);
    layout->addWidget(link);
    layout->addWidget(dev);
    layout->addWidget(line);
    layout->setAlignment(l, Qt::AlignHCenter);
    layout->setAlignment(t, Qt::AlignHCenter);
    layout->setAlignment(copyright, Qt::AlignHCenter);
    layout->setAlignment(info, Qt::AlignHCenter);
    layout->setAlignment(link, Qt::AlignHCenter);

    for (auto dev : developers)
    {
        QLabel* devLabel = new QLabel(dev);
        layout->addWidget(devLabel);
        layout->setAlignment(devLabel, Qt::AlignHCenter);
    }

    this->setLayout(layout);
    this->setWindowFlags(flags);
    this->setWindowTitle(window_title);
}
