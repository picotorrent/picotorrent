#include "downloadspreferencespage.hpp"
#include "ui_downloadspreferencespage.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "core/configuration.hpp"
#include "translator.hpp"
#include "widgets/sunkenline.hpp"

using pt::DownloadsPreferencesPage;

DownloadsPreferencesPage::DownloadsPreferencesPage(QWidget* parent)
    : QWidget(parent),
    m_ui(new Ui::DownloadsPreferencesPage())
{
    m_ui->setupUi(this);

    // Translate
    m_ui->transfersGroup->setTitle(i18n("transfers"));
    m_ui->savePathLabel->setText(i18n("save_path"));
    m_ui->savePathBrowse->setText(i18n("browse"));
    m_ui->moveCompleted->setText(i18n("move_completed_downloads"));
    m_ui->moveCompletedBrowse->setText(i18n("browse"));
    m_ui->moveCompletedDefaultOnly->setText(i18n("only_move_from_default_save_path"));
    m_ui->pauseDiskSpaceLow->setText(i18n("pause_on_low_disk_space"));
    m_ui->limitsGroup->setTitle(i18n("limits"));
    m_ui->dlLimitEnable->setText(i18n("dl_limit"));
    m_ui->dlLimitUnit->setText(i18n("kb_s"));
    m_ui->ulLimitEnable->setText(i18n("ul_limit"));
    m_ui->ulLimitUnit->setText(i18n("kb_s"));
    m_ui->activeTotalLabel->setText(i18n("total_active"));
    m_ui->activeDownloadsLabel->setText(i18n("active_downloads"));
    m_ui->activeSeedsLabel->setText(i18n("active_seeds"));

    connect(m_ui->moveCompleted, &QCheckBox::stateChanged,
            [this](int state)
            {
                m_ui->moveCompletedBrowse->setEnabled(state == Qt::Checked);
                m_ui->moveCompletedPath->setEnabled(state == Qt::Checked);
                m_ui->moveCompletedDefaultOnly->setEnabled(state == Qt::Checked);
            });

    connect(m_ui->dlLimitEnable, &QCheckBox::stateChanged,
            [this](int state)
            {
                m_ui->dlLimit->setEnabled(state == Qt::Checked);
            });

    connect(m_ui->ulLimitEnable, &QCheckBox::stateChanged,
            [this](int state)
            {
                m_ui->ulLimit->setEnabled(state == Qt::Checked);
            });

    // Connect save path (and move completed path) changing
    connect(m_ui->savePathBrowse,      &QPushButton::clicked,
            [this]()
            {
                showPathDialog(m_ui->savePath);
            });

    connect(m_ui->moveCompletedBrowse, &QPushButton::clicked,
            [this]()
            {
                showPathDialog(m_ui->moveCompletedPath);
            });
}

DownloadsPreferencesPage::~DownloadsPreferencesPage()
{
    delete m_ui;
}

void DownloadsPreferencesPage::loadConfig(std::shared_ptr<pt::Configuration> cfg)
{
    bool moveCompleted = cfg->getBool("move_completed_downloads");
    bool onlyFromDefault = cfg->getBool("move_completed_downloads_from_default_only");
    bool enableDownLimit = cfg->getBool("enable_download_rate_limit");
    bool enableUpLimit = cfg->getBool("enable_upload_rate_limit");

    if (!moveCompleted)
    {
        m_ui->moveCompletedBrowse->setEnabled(false);
        m_ui->moveCompletedPath->setEnabled(false);
        m_ui->moveCompletedDefaultOnly->setEnabled(false);
    }

    m_ui->savePath->setText(QString::fromStdString(cfg->getString("default_save_path")));
    m_ui->moveCompleted->setChecked(moveCompleted);
    m_ui->moveCompletedPath->setText(QString::fromStdString(cfg->getString("move_completed_downloads_path")));
    m_ui->moveCompletedDefaultOnly->setChecked(onlyFromDefault);
    m_ui->pauseDiskSpaceLow->setChecked(cfg->getBool("pause_on_low_disk_space"));

    m_ui->dlLimit->setEnabled(enableDownLimit);
    m_ui->dlLimit->setValue(cfg->getInt("download_rate_limit"));
    m_ui->dlLimitEnable->setChecked(enableDownLimit);
    m_ui->ulLimit->setEnabled(enableUpLimit);
    m_ui->ulLimit->setValue(cfg->getInt("upload_rate_limit"));
    m_ui->ulLimitEnable->setChecked(enableUpLimit);

    m_ui->activeTotal->setValue(cfg->getInt("active_limit"));
    m_ui->activeDownloads->setValue(cfg->getInt("active_downloads"));
    m_ui->activeSeeds->setValue(cfg->getInt("active_seeds"));
}

void DownloadsPreferencesPage::saveConfig(std::shared_ptr<pt::Configuration> cfg)
{
    cfg->setString("default_save_path", m_ui->savePath->text().toStdString());
    cfg->setBool("move_completed_downloads", m_ui->moveCompleted->checkState() == Qt::Checked);
    cfg->setString("move_completed_downloads_path", m_ui->moveCompletedPath->text().toStdString());
    cfg->setBool("move_completed_downloads_from_default_only", m_ui->moveCompletedDefaultOnly->checkState() == Qt::Checked);
    cfg->setBool("pause_on_low_disk_space", m_ui->pauseDiskSpaceLow->checkState() == Qt::Checked);
    cfg->setInt("download_rate_limit", m_ui->dlLimit->value());
    cfg->setInt("upload_rate_limit", m_ui->ulLimit->value());
    cfg->setBool("enable_download_rate_limit", m_ui->dlLimitEnable->checkState() == Qt::Checked);
    cfg->setBool("enable_upload_rate_limit", m_ui->ulLimitEnable->checkState() == Qt::Checked);
    cfg->setInt("active_limit", m_ui->activeTotal->value());
    cfg->setInt("active_downloads", m_ui->activeDownloads->value());
    cfg->setInt("active_seeds", m_ui->activeSeeds->value());
}

void DownloadsPreferencesPage::showPathDialog(QLineEdit* target)
{
    auto dlg = new QFileDialog(this);
    dlg->setFileMode(QFileDialog::Directory);
    dlg->setOption(QFileDialog::ShowDirsOnly);

    if (!target->text().isEmpty())
    {
        dlg->setDirectory(target->text());
    }

    dlg->open();

    QObject::connect(
        dlg, &QDialog::finished,
        [dlg, target](int result)
        {
            if (result)
            {
                QStringList files  = dlg->selectedFiles();
                QString nativePath = QDir::toNativeSeparators(files.at(0));

                target->setText(nativePath);
            }

            dlg->deleteLater();
        });
}
