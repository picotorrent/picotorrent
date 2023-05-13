#include "preferencesgeneralpage.hpp"

#include <strsafe.h>

#include <boost/log/trivial.hpp>

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
            BOOST_LOG_TRIVIAL(warning) << "PicoTorrent could not be registered to run at start-up. Error: " << GetLastError();
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

PreferencesGeneralPage::PreferencesGeneralPage(wxWindow* parent, std::shared_ptr<pt::Core::Configuration> cfg)
    : wxPanel(parent),
    m_cfg(cfg)
{
    wxStaticBoxSizer* uiSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("user_interface"));
    wxFlexGridSizer* uiGrid = new wxFlexGridSizer(2, FromDIP(10), FromDIP(10));
    m_language = new wxChoice(uiSizer->GetStaticBox(), wxID_ANY);
    m_theme = new wxChoice(uiSizer->GetStaticBox(), wxID_ANY);
    uiGrid->AddGrowableCol(1, 1);
    uiGrid->Add(new wxStaticText(uiSizer->GetStaticBox(), wxID_ANY, i18n("language")), 0, wxALIGN_CENTER_VERTICAL);
    uiGrid->Add(m_language, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    uiGrid->Add(new wxStaticText(uiSizer->GetStaticBox(), wxID_ANY, i18n("theme")), 0, wxALIGN_CENTER_VERTICAL);
    uiGrid->Add(m_theme, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    uiSizer->Add(uiGrid, 1, wxEXPAND | wxALL, FromDIP(5));

    wxStaticBoxSizer* miscSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("miscellaneous"));
    wxFlexGridSizer* miscGrid = new wxFlexGridSizer(2, FromDIP(10), FromDIP(10));

    m_labelColor = new wxCheckBox(miscSizer->GetStaticBox(), wxID_ANY, i18n("use_label_color_as_background_in_torrent_list"));
    m_skipAddTorrentDialog = new wxCheckBox(miscSizer->GetStaticBox(), wxID_ANY, i18n("skip_add_torrent_dialog"));
    m_autoStart = new wxCheckBox(miscSizer->GetStaticBox(), wxID_ANY, i18n("start_with_windows"));
    m_startPosition = new wxChoice(miscSizer->GetStaticBox(), wxID_ANY);

    miscGrid->AddGrowableCol(1, 1);
    miscGrid->Add(m_labelColor, 0, wxALIGN_CENTER_VERTICAL);
    miscGrid->Add(0, 0);
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
        int pos = m_language->Append(lang.name, new ClientData<std::string>(lang.locale));

        if (lang.locale == Translator::GetInstance().GetLocale())
        {
            m_language->SetSelection(pos);
        }
    }
    
    m_theme->Append(i18n("follow_system_theme"), new ClientData<std::string>("system"));
    m_theme->Append(i18n("light_theme"), new ClientData<std::string>("light"));
    if (m_cfg->Get<std::string>("theme_id").value_or("system") == "light")
    {
        m_theme->SetSelection(1);
    }
    else
    {
        m_theme->SetSelection(0);
    }

    m_labelColor->SetValue(m_cfg->Get<bool>("use_label_as_list_bgcolor").value());
    m_skipAddTorrentDialog->SetValue(m_cfg->Get<bool>("skip_add_torrent_dialog").value());

    AutoRunKey key;
    if (key.Exists())
    {
        m_autoStart->SetValue(true);
    }

    m_startPosition->Append(i18n("normal"), new ClientData<Configuration::WindowState>(Configuration::WindowState::Normal));
    m_startPosition->Append(i18n("minimized"), new ClientData<Configuration::WindowState>(Configuration::WindowState::Minimized));
    m_startPosition->Append(i18n("hidden"), new ClientData<Configuration::WindowState>(Configuration::WindowState::Hidden));
    m_startPosition->Append(i18n("maximized"), new ClientData<Configuration::WindowState>(Configuration::WindowState::Maximized));
    m_startPosition->SetSelection(m_cfg->Get<int>("start_position").value());

    // Notification area
    m_showNotificationIcon->SetValue(m_cfg->Get<bool>("show_in_notification_area").value());
    m_minimizeNotification->SetValue(m_cfg->Get<bool>("minimize_to_notification_area").value());
    m_closeNotification->SetValue(m_cfg->Get<bool>("close_to_notification_area").value());

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

void PreferencesGeneralPage::Save(bool* restartRequired)
{
    int langIndex = m_language->GetSelection();
    ClientData<std::string>* langData = langIndex >= 0
        ? reinterpret_cast<ClientData<std::string>*>(m_language->GetClientObject(langIndex))
        : nullptr;

    int themeIndex = m_theme->GetSelection();
    ClientData<std::string> *themeData = themeIndex >= 0
        ? reinterpret_cast<ClientData<std::string> *>(m_theme->GetClientObject(themeIndex))
        : nullptr;

    int startPosIndex = m_startPosition->GetSelection();
    ClientData<Configuration::WindowState>* startPosData = reinterpret_cast<ClientData<Configuration::WindowState>*>(m_startPosition->GetClientObject(startPosIndex));

    if (langData != nullptr)
    {
        if (langData->GetValue() != m_cfg->Get<std::string>("locale_name"))
        {
            *restartRequired = true;
        }

        m_cfg->Set("locale_name", langData->GetValue());
    }

    if (themeData != nullptr)
    {
        if (themeData->GetValue() != m_cfg->Get<std::string>("theme_id"))
        {
            *restartRequired = true;
        }
        m_cfg->Set("theme_id", themeData->GetValue());
    }

    if (startPosData != nullptr)
    {
        m_cfg->Set("start_position", static_cast<int>(startPosData->GetValue()));
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

    m_cfg->Set("use_label_as_list_bgcolor", m_labelColor->GetValue());
    m_cfg->Set("skip_add_torrent_dialog", m_skipAddTorrentDialog->GetValue());
    m_cfg->Set("show_in_notification_area", m_showNotificationIcon->GetValue());
    m_cfg->Set("minimize_to_notification_area", m_minimizeNotification->GetValue());
    m_cfg->Set("close_to_notification_area", m_closeNotification->GetValue());
}
