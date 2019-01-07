#include "aboutdialog.hpp"

#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>

#include "buildinfo.hpp"
#include "translator.hpp"

using pt::AboutDialog;

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    QString info_str = i18n("picotorrent_description");
    QString heading = QString::asprintf(i18n("picotorrent_v_format").toStdString().c_str(), BuildInfo::version().toStdString().c_str());
    QString dev_heading = "Developers";
    QString window_title = i18n("about_picotorrent");
    QList<QString> developers;
    developers.push_back(QString("Viktor Elofsson"));

    // All widgets defined
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addSpacing(3);

    QIcon* ico = new QIcon(":res/app.ico");
    QLabel* icon = new QLabel();
    icon->setPixmap(ico->pixmap(64, 64));

    QLabel* heading_l = new QLabel(heading);
    heading_l->setStyleSheet("QLabel { font-size: 18px; }");

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
    layout->addWidget(icon);
    layout->addWidget(heading_l);
    layout->addWidget(copyright);
    layout->addWidget(info);
    layout->addWidget(link);
    layout->addWidget(dev);
    layout->addWidget(line);
    layout->setAlignment(icon, Qt::AlignHCenter);
    layout->setAlignment(heading_l, Qt::AlignHCenter);
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
