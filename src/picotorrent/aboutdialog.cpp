#include "aboutdialog.hpp"
#include "ui_aboutdialog.h"

#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>

#include "buildinfo.hpp"
#include "translator.hpp"

using pt::AboutDialog;

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent),
    m_ui(new Ui::AboutDialog())
{
    QIcon ic(":res/app.ico");

    m_ui->setupUi(this);
    m_ui->icon->setPixmap(ic.pixmap(64, 64));
    m_ui->heading->setText(i18n("picotorrent_v_format").arg(BuildInfo::version()));
    m_ui->info->setText(i18n("picotorrent_description"));

    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    this->setFixedSize(this->sizeHint());
    this->setWindowFlags(flags);
    this->setWindowTitle(i18n("about_picotorrent"));
}

AboutDialog::~AboutDialog()
{
    delete m_ui;
}
