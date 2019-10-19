#include "generalsectionwidget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include <strsafe.h>

#include "core/configuration.hpp"
#include "translator.hpp"

using pt::GeneralSectionWidget;

struct AutoRunKey
{
    AutoRunKey()
    {
        RegCreateKeyEx(
            HKEY_CURRENT_USER,
            TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
            0,
            NULL,
            0,
            KEY_READ | KEY_WRITE,
            NULL,
            &m_key,
            NULL);
    }

    void Create()
    {
        TCHAR path[MAX_PATH];
        TCHAR quoted[MAX_PATH];
        GetModuleFileName(NULL, path, ARRAYSIZE(path));
        StringCchPrintf(quoted, ARRAYSIZE(quoted), L"\"%s\"", path);

        std::wstring p(quoted);

        UINT res = RegSetValueEx(
            m_key,
            L"PicoTorrent",
            0,
            REG_SZ,
            (const BYTE*)p.c_str(),
            (DWORD)((p.size() + 1) * sizeof(wchar_t)));

        if (res != ERROR_SUCCESS)
        {
            DWORD err = GetLastError();
            // LOG(warning) << "PicoTorrent could not be registered to run at start-up. Error: " << err;
        }
    }

    void Delete()
    {
        RegDeleteValue(
            m_key,
            TEXT("PicoTorrent"));
    }

    bool Exists()
    {
        return RegQueryValueEx(
            m_key,
            TEXT("PicoTorrent"),
            NULL,
            NULL,
            NULL,
            NULL) == ERROR_SUCCESS;;
    }

    ~AutoRunKey()
    {
        if (m_key != NULL)
        {
            RegCloseKey(m_key);
        }
    }

private:
    HKEY m_key;
};

GeneralSectionWidget::GeneralSectionWidget()
{
    createUi();

    connect(
        m_showInNotificationArea,
        &QCheckBox::stateChanged,
        this,
        &GeneralSectionWidget::onShowInNotificationAreaChanged);
}

void GeneralSectionWidget::loadConfig(std::shared_ptr<pt::Configuration> cfg)
{
    int currentLanguage = cfg->getInt("language_id");

    for (int i = 0; i < m_languages->count(); i++)
    {
        if (m_languages->itemData(i).toInt() == currentLanguage)
        {
            m_languages->setCurrentIndex(i);
            break;
        }
    }

    m_startPosition->setCurrentIndex(cfg->getInt("start_position"));

    AutoRunKey key;
    if (key.Exists())
    {
        m_startWithWindows->setChecked(true);
    }

    m_skipAddTorrentDialog->setChecked(cfg->getBool("skip_add_torrent_dialog"));
    m_showInNotificationArea->setChecked(cfg->getBool("show_in_notification_area"));
    m_minimizeToNotificationArea->setChecked(cfg->getBool("minimize_to_notification_area"));
    m_closeToNotificationArea->setChecked(cfg->getBool("close_to_notification_area"));
    m_geoipEnabled->setChecked(cfg->getBool("geoip.enabled"));
}

void GeneralSectionWidget::saveConfig(std::shared_ptr<pt::Configuration> cfg, bool* requiresRestart)
{
    auto langIndex = m_languages->currentIndex();
    auto langData = m_languages->itemData(langIndex);

    if (cfg->getInt("language_id") != langData.toInt())
    {
        *requiresRestart = true;
    }

    cfg->setInt("language_id", langData.toInt());
    cfg->setInt("start_position", m_startPosition->currentIndex());
    cfg->setBool("skip_add_torrent_dialog", m_skipAddTorrentDialog->checkState() == Qt::Checked);
    cfg->setBool("show_in_notification_area", m_showInNotificationArea->checkState() == Qt::Checked);
    cfg->setBool("minimize_to_notification_area", m_minimizeToNotificationArea->checkState() == Qt::Checked);
    cfg->setBool("close_to_notification_area", m_closeToNotificationArea->checkState() == Qt::Checked);
    cfg->setBool("geoip.enabled", m_geoipEnabled->checkState() == Qt::Checked);

    AutoRunKey key;

    if (key.Exists() && !(m_startWithWindows->checkState() == Qt::Checked))
    {
        key.Delete();
    }

    if (!key.Exists() && (m_startWithWindows->checkState() == Qt::Checked))
    {
        key.Create();
    }
}

void GeneralSectionWidget::createUi()
{
    m_languages = new QComboBox();
    m_skipAddTorrentDialog = new QCheckBox(i18n("skip_add_torrent_dialog"));
    m_startWithWindows = new QCheckBox(i18n("start_with_windows"));
    m_startPosition = new QComboBox();
    m_showInNotificationArea = new QCheckBox(i18n("show_picotorrent_in_notification_area"));
    m_minimizeToNotificationArea = new QCheckBox(i18n("minimize_to_notification_area"));
    m_closeToNotificationArea = new QCheckBox(i18n("close_to_notification_area"));
    m_geoipEnabled = new QCheckBox(i18n("enable_geoip"));

    Translator& tr = Translator::instance();

    for (auto& lang : tr.languages())
    {
        m_languages->addItem(lang.name, lang.code);
    }

    m_startPosition->addItem(i18n("normal"), static_cast<int>(Configuration::WindowState::Normal));
    m_startPosition->addItem(i18n("minimized"), static_cast<int>(Configuration::WindowState::Minimized));
    m_startPosition->addItem(i18n("hidden"), static_cast<int>(Configuration::WindowState::Hidden));
    m_startPosition->addItem(i18n("maximized"), static_cast<int>(Configuration::WindowState::Maximized));

    auto uiGrid = new QGridLayout();
    uiGrid->addWidget(new QLabel(i18n("language")), 0, 0);
    uiGrid->addWidget(m_languages, 0, 1);

    auto miscGrid = new QGridLayout();
    miscGrid->addWidget(m_geoipEnabled, 0, 0);
    miscGrid->addWidget(m_skipAddTorrentDialog, 1, 0);
    miscGrid->addWidget(m_startWithWindows, 2, 0);
    miscGrid->addWidget(new QLabel(i18n("start_position")), 3, 0);
    miscGrid->addWidget(m_startPosition, 3, 1);

    auto notifIndentedLayout = new QVBoxLayout();
    notifIndentedLayout->addWidget(m_minimizeToNotificationArea);
    notifIndentedLayout->addWidget(m_closeToNotificationArea);
    notifIndentedLayout->setContentsMargins(10, 0, 0, 0);

    auto notifLayout = new QVBoxLayout();
    notifLayout->addWidget(m_showInNotificationArea);
    notifLayout->addLayout(notifIndentedLayout);

    auto uiGroup = new QGroupBox(i18n("user_interface"));
    uiGroup->setLayout(uiGrid);

    auto miscGroup = new QGroupBox(i18n("miscellaneous"));
    miscGroup->setLayout(miscGrid);

    auto notifGroup = new QGroupBox(i18n("notification_area"));
    notifGroup->setLayout(notifLayout);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(uiGroup);
    layout->addWidget(miscGroup);
    layout->addWidget(notifGroup);
    layout->addStretch();
    layout->setMargin(0);

    this->setLayout(layout);
}

void GeneralSectionWidget::onShowInNotificationAreaChanged(int state)
{
    m_minimizeToNotificationArea->setEnabled(state == Qt::Checked);
    m_closeToNotificationArea->setEnabled(state == Qt::Checked);
}
