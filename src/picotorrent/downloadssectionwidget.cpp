#include "downloadssectionwidget.hpp"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "configuration.hpp"
#include "translator.hpp"

using pt::DownloadsSectionWidget;

DownloadsSectionWidget::DownloadsSectionWidget()
{
    createUi();

    connect(
        m_moveCompleted,
        &QCheckBox::stateChanged,
        [this](int state)
        {
            m_moveCompletedBrowse->setEnabled(state == Qt::Checked);
            m_moveCompletedPath->setEnabled(state == Qt::Checked);
            m_onlyMoveFromDefault->setEnabled(state == Qt::Checked);
        });

    connect(
        m_downLimitEnable,
        &QCheckBox::stateChanged,
        [this](int state)
        {
            m_downLimit->setEnabled(state == Qt::Checked);
        });

    connect(
        m_upLimitEnable,
        &QCheckBox::stateChanged,
        [this](int state)
        {
            m_upLimit->setEnabled(state == Qt::Checked);
        });
}

void DownloadsSectionWidget::loadConfig(std::shared_ptr<pt::Configuration> cfg)
{
    bool moveCompleted = cfg->getBool("move_completed_downloads");
    bool onlyFromDefault = cfg->getBool("move_completed_downloads_from_default_only");
    bool enableDownLimit = cfg->getBool("enable_download_rate_limit");
    bool enableUpLimit = cfg->getBool("enable_upload_rate_limit");

    if (!moveCompleted)
    {
        m_moveCompletedBrowse->setEnabled(false);
        m_moveCompletedPath->setEnabled(false);
        m_onlyMoveFromDefault->setEnabled(false);
    }

    m_savePath->setText(QString::fromStdString(cfg->getString("default_save_path")));
    m_moveCompleted->setChecked(moveCompleted);
    m_moveCompletedPath->setText(QString::fromStdString(cfg->getString("move_completed_downloads_path")));
    m_onlyMoveFromDefault->setChecked(onlyFromDefault);

    m_downLimit->setEnabled(enableDownLimit);
    m_downLimit->setText(QString::number(cfg->getInt("download_rate_limit")));
    m_downLimitEnable->setChecked(enableDownLimit);
    m_upLimit->setEnabled(enableUpLimit);
    m_upLimit->setText(QString::number(cfg->getInt("upload_rate_limit")));
    m_upLimitEnable->setChecked(enableUpLimit);

    m_totalActive->setText(QString::number(cfg->getInt("active_limit")));
    m_activeDownloads->setText(QString::number(cfg->getInt("active_downloads")));
    m_activeSeeds->setText(QString::number(cfg->getInt("active_seeds")));
}

void DownloadsSectionWidget::saveConfig(std::shared_ptr<pt::Configuration> cfg)
{
    cfg->setString("default_save_path", m_savePath->text().toStdString());
    cfg->setBool("move_completed_downloads", m_moveCompleted->checkState() == Qt::Checked);
    cfg->setString("move_completed_downloads_path", m_moveCompletedPath->text().toStdString());
    cfg->setBool("move_completed_downloads_from_default_only", m_onlyMoveFromDefault->checkState() == Qt::Checked);
    cfg->setInt("download_rate_limit", m_downLimit->text().toInt());
    cfg->setInt("upload_rate_limit", m_upLimit->text().toInt());
    cfg->setBool("enable_download_rate_limit", m_downLimitEnable->checkState() == Qt::Checked);
    cfg->setBool("enable_upload_rate_limit", m_upLimitEnable->checkState() == Qt::Checked);
    cfg->setInt("active_limit", m_totalActive->text().toInt());
    cfg->setInt("active_downloads", m_activeDownloads->text().toInt());
    cfg->setInt("active_seeds", m_activeSeeds->text().toInt());
}

void DownloadsSectionWidget::createUi()
{
    m_savePath = new QLineEdit();
    m_savePathBrowse = new QPushButton("...");
    m_moveCompleted = new QCheckBox(i18n("move_completed_downloads"));
    m_moveCompletedPath = new QLineEdit();
    m_moveCompletedBrowse = new QPushButton("...");
    m_onlyMoveFromDefault = new QCheckBox(i18n("only_move_from_default_save_path"));
    m_downLimit = new QLineEdit();
    m_downLimitEnable = new QCheckBox(i18n("dl_limit"));
    m_upLimit = new QLineEdit();
    m_upLimitEnable = new QCheckBox(i18n("ul_limit"));
    m_totalActive = new QLineEdit();
    m_activeDownloads = new QLineEdit();
    m_activeSeeds = new QLineEdit();

    m_savePathBrowse->setMaximumWidth(30);
    m_moveCompletedBrowse->setMaximumWidth(30);

    m_downLimit->setAlignment(Qt::AlignRight);
    m_downLimit->setMaximumWidth(50);
    m_downLimit->setValidator(new QIntValidator());

    m_upLimit->setAlignment(Qt::AlignRight);
    m_upLimit->setMaximumWidth(50);
    m_upLimit->setValidator(new QIntValidator());

    m_totalActive->setAlignment(Qt::AlignRight);
    m_totalActive->setMaximumWidth(50);
    m_totalActive->setValidator(new QIntValidator());

    m_activeDownloads->setAlignment(Qt::AlignRight);
    m_activeDownloads->setMaximumWidth(50);
    m_activeDownloads->setValidator(new QIntValidator());

    m_activeSeeds->setAlignment(Qt::AlignRight);
    m_activeSeeds->setMaximumWidth(50);
    m_activeSeeds->setValidator(new QIntValidator());

    auto savePathLayout = new QHBoxLayout();
    savePathLayout->addWidget(new QLabel(i18n("save_path")));
    savePathLayout->addWidget(m_savePath);
    savePathLayout->addWidget(m_savePathBrowse);

    auto moveCompletedSublayout = new QHBoxLayout();
    moveCompletedSublayout->addWidget(m_moveCompletedPath);
    moveCompletedSublayout->addWidget(m_moveCompletedBrowse);

    auto moveCompletedLayout = new QVBoxLayout();
    moveCompletedLayout->addLayout(moveCompletedSublayout);
    moveCompletedLayout->addWidget(m_onlyMoveFromDefault);
    moveCompletedLayout->setContentsMargins(10, 0, 0, 0);

    auto transfersLayout = new QVBoxLayout();
    transfersLayout->addLayout(savePathLayout);
    transfersLayout->addWidget(m_moveCompleted);
    transfersLayout->addLayout(moveCompletedLayout);

    auto transfersGroup = new QGroupBox(i18n("transfers"));
    transfersGroup->setLayout(transfersLayout);

    auto hl = new QFrame(this);
    hl->setFrameShape(QFrame::HLine);
    hl->setFrameShadow(QFrame::Sunken);

    auto limitsLayout = new QGridLayout();
    limitsLayout->addWidget(m_downLimitEnable,                    0, 0);
    limitsLayout->addWidget(m_downLimit,                          0, 1);
    limitsLayout->addWidget(new QLabel("KB/s"),                   0, 2);
    limitsLayout->addWidget(m_upLimitEnable,                      1, 0);
    limitsLayout->addWidget(m_upLimit,                            1, 1);
    limitsLayout->addWidget(new QLabel("KB/s"),                   1, 2);
    limitsLayout->addWidget(hl,                                   2, 0, 1, 3);
    limitsLayout->addWidget(new QLabel(i18n("total_active")),     3, 0);
    limitsLayout->addWidget(m_totalActive,                        3, 1);
    limitsLayout->addWidget(new QLabel(i18n("active_downloads")), 4, 0);
    limitsLayout->addWidget(m_activeDownloads,                    4, 1);
    limitsLayout->addWidget(new QLabel(i18n("active_seeds")),     5, 0);
    limitsLayout->addWidget(m_activeSeeds,                        5, 1);
    limitsLayout->setColumnStretch(0, 1);

    auto limitsGroup = new QGroupBox(i18n("limits"));
    limitsGroup->setLayout(limitsLayout);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(transfersGroup);
    layout->addWidget(limitsGroup);
    layout->addStretch();
    layout->setMargin(0);

    this->setLayout(layout);
}
