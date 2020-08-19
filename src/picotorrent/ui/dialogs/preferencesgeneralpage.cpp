#include "preferencesgeneralpage.hpp"

#include <strsafe.h>

#include <loguru.hpp>

#include "../clientdata.hpp"
#include "../../core/configuration.hpp"
#include "../../core/utils.hpp"
#include "../translator.hpp"

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
            LOG_F(WARNING, "PicoTorrent could not be registered to run at start-up. Error: %d", GetLastError());
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

using pt::Core::Configuration;
using pt::UI::Dialogs::PreferencesGeneralPage;

PreferencesGeneralPage::PreferencesGeneralPage(wxWindow* parent, std::shared_ptr<Core::Configuration> cfg)
    : wxPanel(parent),
    m_cfg(cfg)
{
    wxStaticBoxSizer* uiSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("user_interface"));
    wxFlexGridSizer* uiGrid = new wxFlexGridSizer(2, FromDIP(10), FromDIP(10));
    m_language = new wxChoice(uiSizer->GetStaticBox(), wxID_ANY);
    uiGrid->AddGrowableCol(1, 1);
    uiGrid->Add(new wxStaticText(uiSizer->GetStaticBox(), wxID_ANY, i18n("language")), 0, wxALIGN_CENTER_VERTICAL);
    uiGrid->Add(m_language, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    uiSizer->Add(uiGrid, 1, wxEXPAND | wxALL, FromDIP(5));

    wxStaticBoxSizer* miscSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("miscellaneous"));
    wxFlexGridSizer* miscGrid = new wxFlexGridSizer(2, FromDIP(10), FromDIP(10));

    m_skipAddTorrentDialog = new wxCheckBox(miscSizer->GetStaticBox(), wxID_ANY, i18n("skip_add_torrent_dialog"));
    m_autoStart = new wxCheckBox(miscSizer->GetStaticBox(), wxID_ANY, i18n("start_with_windows"));
    m_startPosition = new wxChoice(miscSizer->GetStaticBox(), wxID_ANY);

    miscGrid->AddGrowableCol(1, 1);
    miscGrid->Add(m_skipAddTorrentDialog, 0, wxALIGN_CENTER_VERTICAL);
    miscGrid->Add(0, 0);
    miscGrid->Add(m_autoStart, 0, wxALIGN_CENTER_VERTICAL);
    miscGrid->Add(0, 0);
    miscGrid->Add(new wxStaticText(miscSizer->GetStaticBox(), wxID_ANY, i18n("start_position")), 0, wxALIGN_CENTER_VERTICAL);
    miscGrid->Add(m_startPosition, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    miscSizer->Add(miscGrid, 1, wxEXPAND | wxALL, FromDIP(5));

    wxStaticBoxSizer* notifSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("notification_area"));
    wxFlexGridSizer* notifGrid = new wxFlexGridSizer(1, FromDIP(10), FromDIP(10));

    m_showNotificationIcon = new wxCheckBox(notifSizer->GetStaticBox(), wxID_ANY, i18n("show_picotorrent_in_notification_area"));
    m_minimizeNotification = new wxCheckBox(notifSizer->GetStaticBox(), wxID_ANY, i18n("minimize_to_notification_area"));
    m_closeNotification = new wxCheckBox(notifSizer->GetStaticBox(), wxID_ANY, i18n("close_to_notification_area"));

    notifGrid->AddGrowableCol(0, 1);
    notifGrid->Add(m_showNotificationIcon);
    notifGrid->Add(m_minimizeNotification, 0, wxLEFT, FromDIP(15));
    notifGrid->Add(m_closeNotification, 0, wxLEFT, FromDIP(15));
    notifSizer->Add(notifGrid, 1, wxEXPAND | wxALL, FromDIP(5));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(uiSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(miscSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(notifSizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    this->SetSizerAndFit(sizer);

    for (auto& lang : Translator::GetInstance().Languages())
    {
        int pos = m_language->Append(lang.name, new ClientData<int>(lang.code));

        if (lang.code == m_cfg->GetInt("language_id"))
        {
            m_language->SetSelection(pos);
        }
    }

    m_skipAddTorrentDialog->SetValue(m_cfg->GetBool("skip_add_torrent_dialog"));

    AutoRunKey key;
    if (key.Exists())
    {
        m_autoStart->SetValue(true);
    }

    m_startPosition->Insert(i18n("normal"), Configuration::WindowState::Normal, new ClientData<Configuration::WindowState>(Configuration::WindowState::Normal));
    m_startPosition->Insert(i18n("minimized"), Configuration::WindowState::Minimized, new ClientData<Configuration::WindowState>(Configuration::WindowState::Minimized));
    m_startPosition->Insert(i18n("hidden"), Configuration::WindowState::Hidden, new ClientData<Configuration::WindowState>(Configuration::WindowState::Hidden));
    m_startPosition->Insert(i18n("maximized"), Configuration::WindowState::Maximized, new ClientData<Configuration::WindowState>(Configuration::WindowState::Maximized));
    m_startPosition->SetSelection(m_cfg->GetInt("start_position"));

    // Notification area
    m_showNotificationIcon->SetValue(m_cfg->GetBool("show_in_notification_area"));
    m_minimizeNotification->SetValue(m_cfg->GetBool("minimize_to_notification_area"));
    m_closeNotification->SetValue(m_cfg->GetBool("close_to_notification_area"));

    m_showNotificationIcon->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&)
        {
            m_minimizeNotification->Enable(m_showNotificationIcon->IsChecked());
            m_closeNotification->Enable(m_showNotificationIcon->IsChecked());
        });

    m_minimizeNotification->Enable(m_showNotificationIcon->IsChecked());
    m_closeNotification->Enable(m_showNotificationIcon->IsChecked());
}

PreferencesGeneralPage::~PreferencesGeneralPage()
{
}

bool PreferencesGeneralPage::IsValid()
{
    return true;
}

void PreferencesGeneralPage::Save()
{
    int langIndex = m_language->GetSelection();
    ClientData<int>* langData = reinterpret_cast<ClientData<int>*>(m_language->GetClientObject(langIndex));

    int startPosIndex = m_startPosition->GetSelection();
    ClientData<Configuration::WindowState>* startPosData = reinterpret_cast<ClientData<Configuration::WindowState>*>(m_startPosition->GetClientObject(startPosIndex));

    if (langData != nullptr)
    {
        // Sometimes we see crashes where langData is null. Can't reproduce,
        // but the simple fix is to null check.
        m_cfg->SetInt("language_id", static_cast<int>(langData->GetValue()));
    }

    if (startPosData != nullptr)
    {
        m_cfg->SetInt("start_position", startPosData->GetValue());
    }

    {
        AutoRunKey key;

        if (key.Exists() && !m_autoStart->GetValue())
        {
            key.Delete();
        }

        if (!key.Exists() && m_autoStart->GetValue())
        {
            key.Create();
        }
    }

    m_cfg->SetBool("skip_add_torrent_dialog", m_skipAddTorrentDialog->GetValue());
    m_cfg->SetBool("show_in_notification_area", m_showNotificationIcon->GetValue());
    m_cfg->SetBool("minimize_to_notification_area", m_minimizeNotification->GetValue());
    m_cfg->SetBool("close_to_notification_area", m_closeNotification->GetValue());
}
