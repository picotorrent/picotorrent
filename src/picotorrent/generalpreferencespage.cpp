#include "generalpreferencespage.hpp"
#include "ui_generalpreferencespage.h"

#include <filesystem>
#include <fstream>

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include <strsafe.h>

#include "core/configuration.hpp"
#include "core/environment.hpp"
#include "widgets/sunkenline.hpp"
#include "translator.hpp"

namespace fs = std::filesystem;
using pt::GeneralPreferencesPage;

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

GeneralPreferencesPage::GeneralPreferencesPage(QWidget* parent)
    : QWidget(parent),
    m_ui(new Ui::GeneralPreferencesPage())
{
    m_ui->setupUi(this);

    m_ui->userInterfaceGroup->setTitle(i18n("user_interface"));
    m_ui->languageLabel->setText(i18n("language"));
    m_ui->miscGroup->setTitle(i18n("miscellaneous"));
    m_ui->enableAutomaticCrashReporting->setText(i18n("enable_automatic_crash_reporting"));
    m_ui->enableGeoIPLookups->setText(i18n("enable_geoip"));
    m_ui->skipAddTorrentDialog->setText(i18n("skip_add_torrent_dialog"));
    m_ui->startWithWindows->setText(i18n("start_with_windows"));
    m_ui->startPositionLabel->setText(i18n("start_position"));
    m_ui->notificationGroup->setTitle(i18n("notification_area"));
    m_ui->showInNotificationArea->setText(i18n("show_picotorrent_in_notification_area"));
    m_ui->minimizeToNotificationArea->setText(i18n("minimize_to_notification_area"));
    m_ui->closeToNotificationArea->setText(i18n("close_to_notification_area"));

    Translator& tr = Translator::instance();

    for (auto& lang : tr.languages())
    {
        m_ui->languages->addItem(lang.name, lang.code);
    }

    m_ui->startPosition->addItem(i18n("normal"), static_cast<int>(Configuration::WindowState::Normal));
    m_ui->startPosition->addItem(i18n("minimized"), static_cast<int>(Configuration::WindowState::Minimized));
    m_ui->startPosition->addItem(i18n("hidden"), static_cast<int>(Configuration::WindowState::Hidden));
    m_ui->startPosition->addItem(i18n("maximized"), static_cast<int>(Configuration::WindowState::Maximized));

    connect(
        m_ui->showInNotificationArea,
        &QCheckBox::stateChanged,
        this,
        &GeneralPreferencesPage::onShowInNotificationAreaChanged);
}

GeneralPreferencesPage::~GeneralPreferencesPage()
{
    delete m_ui;
}

void GeneralPreferencesPage::loadConfig(std::shared_ptr<pt::Configuration> cfg, std::shared_ptr<pt::Environment> env)
{
    int currentLanguage = cfg->getInt("language_id");

    for (int i = 0; i < m_ui->languages->count(); i++)
    {
        if (m_ui->languages->itemData(i).toInt() == currentLanguage)
        {
            m_ui->languages->setCurrentIndex(i);
            break;
        }
    }

    m_ui->startPosition->setCurrentIndex(cfg->getInt("start_position"));

    AutoRunKey key;
    if (key.Exists())
    {
        m_ui->startWithWindows->setChecked(true);
    }

    m_ui->skipAddTorrentDialog->setChecked(cfg->getBool("skip_add_torrent_dialog"));
    m_ui->showInNotificationArea->setChecked(cfg->getBool("show_in_notification_area"));
    m_ui->minimizeToNotificationArea->setChecked(cfg->getBool("minimize_to_notification_area"));
    m_ui->closeToNotificationArea->setChecked(cfg->getBool("close_to_notification_area"));
    m_ui->enableAutomaticCrashReporting->setChecked(hasCrashReportingConsent(env));
    m_ui->enableGeoIPLookups->setChecked(cfg->getBool("geoip.enabled"));
}

void GeneralPreferencesPage::saveConfig(std::shared_ptr<pt::Configuration> cfg, std::shared_ptr<pt::Environment> env, bool* requiresRestart)
{
    auto langIndex = m_ui->languages->currentIndex();
    auto langData = m_ui->languages->itemData(langIndex);

    if (cfg->getInt("language_id") != langData.toInt())
    {
        *requiresRestart = true;
    }

    bool prevConsent = hasCrashReportingConsent(env);
    bool newConsent = m_ui->enableAutomaticCrashReporting->isChecked();

    if (prevConsent != newConsent)
    {
        *requiresRestart = true;
    }

    cfg->setInt("language_id", langData.toInt());
    cfg->setInt("start_position", m_ui->startPosition->currentIndex());
    cfg->setBool("skip_add_torrent_dialog", m_ui->skipAddTorrentDialog->checkState() == Qt::Checked);
    cfg->setBool("show_in_notification_area", m_ui->showInNotificationArea->checkState() == Qt::Checked);
    cfg->setBool("minimize_to_notification_area", m_ui->minimizeToNotificationArea->checkState() == Qt::Checked);
    cfg->setBool("close_to_notification_area", m_ui->closeToNotificationArea->checkState() == Qt::Checked);
    cfg->setBool("geoip.enabled", m_ui->enableGeoIPLookups->checkState() == Qt::Checked);

    AutoRunKey key;

    if (key.Exists() && !(m_ui->startWithWindows->checkState() == Qt::Checked))
    {
        key.Delete();
    }

    if (!key.Exists() && (m_ui->startWithWindows->checkState() == Qt::Checked))
    {
        key.Create();
    }

    auto consentFile = env->getApplicationDataPath() / "Crashpad" / "db" / "consent";
    std::ofstream output(consentFile, std::ios::binary);

    char consent[1] = { newConsent ? '1' : '0' };
    output.write(consent, 1);
}

void GeneralPreferencesPage::onShowInNotificationAreaChanged(int state)
{
    m_ui->minimizeToNotificationArea->setEnabled(state == Qt::Checked);
    m_ui->closeToNotificationArea->setEnabled(state == Qt::Checked);
}

bool GeneralPreferencesPage::hasCrashReportingConsent(std::shared_ptr<pt::Environment> env)
{
    auto consentFile = env->getApplicationDataPath() / "Crashpad" / "db" / "consent";

    if (fs::exists(consentFile))
    {
        std::ifstream input(consentFile, std::ios::binary);

        char consent[1] = { '0' };
        input.read(consent, 1);

        return consent[0] == '1';
    }

    return false;
}
