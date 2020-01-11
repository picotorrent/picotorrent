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
    QString heading = i18n("picotorrent_v_format").arg(BuildInfo::version());
    QString window_title = i18n("about_picotorrent");

    // All widgets defined
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addSpacing(3);

    QIcon* ico = new QIcon(":res/app.ico");
    QLabel* icon = new QLabel();
    icon->setPixmap(ico->pixmap(64, 64));

    QLabel* heading_l = new QLabel(heading);
    heading_l->setStyleSheet("QLabel { font-size: 18px; }");

    QString c = QChar(0xA9);
    c.append(" 2015-2020");
    QLabel* copyright = new QLabel(c);

    QLabel* info = new QLabel(info_str);

    QLabel* link = new QLabel("<a href=\"https://picotorrent.org?app\">https://picotorrent.org</a>", this);
    link->setOpenExternalLinks(true);
    link->setTextFormat(Qt::RichText);
    link->setTextInteractionFlags(Qt::TextBrowserInteraction);

    // Adding widgets
    layout->addWidget(icon);
    layout->addWidget(heading_l);
    layout->addWidget(copyright);
    layout->addWidget(info);
    layout->addWidget(link);
    layout->setAlignment(icon, Qt::AlignHCenter);
    layout->setAlignment(heading_l, Qt::AlignHCenter);
    layout->setAlignment(copyright, Qt::AlignHCenter);
    layout->setAlignment(info, Qt::AlignHCenter);
    layout->setAlignment(link, Qt::AlignHCenter);

    this->setLayout(layout);
    this->setWindowFlags(flags);
    this->setWindowTitle(window_title);
}
