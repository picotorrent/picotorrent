#include "generalpage.hpp"

#include <strsafe.h>

#include "clientdata.hpp"
#include "config.hpp"
#include "translator.hpp"

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

using pt::GeneralPage;

GeneralPage::GeneralPage(wxWindow* parent, std::shared_ptr<pt::Configuration> config, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, wxID_ANY),
    m_config(config)
{
    wxStaticBoxSizer* uiSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "user_interface"));
    wxFlexGridSizer* uiGrid = new wxFlexGridSizer(2, 10, 10);
    m_language = new wxChoice(uiSizer->GetStaticBox(), wxID_ANY);
    uiGrid->AddGrowableCol(1, 1);
    uiGrid->Add(new wxStaticText(uiSizer->GetStaticBox(), wxID_ANY, i18n(tr, "language")), 0, wxALIGN_CENTER_VERTICAL);
    uiGrid->Add(m_language, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    uiSizer->Add(uiGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* miscSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "miscellaneous"));
    wxFlexGridSizer* miscGrid = new wxFlexGridSizer(2, 10, 10);

    m_autoStart = new wxCheckBox(miscSizer->GetStaticBox(), wxID_ANY, i18n(tr, "start_with_windows"));
    m_startPosition = new wxChoice(miscSizer->GetStaticBox(), wxID_ANY);

    miscGrid->AddGrowableCol(1, 1);
    miscGrid->Add(m_autoStart, 0, wxALIGN_CENTER_VERTICAL);
    miscGrid->Add(0, 0);
    miscGrid->Add(new wxStaticText(miscSizer->GetStaticBox(), wxID_ANY, i18n(tr, "start_position")), 0, wxALIGN_CENTER_VERTICAL);
    miscGrid->Add(m_startPosition, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    miscSizer->Add(miscGrid, 1, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* notifSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n(tr, "notification_area"));
    wxFlexGridSizer* notifGrid = new wxFlexGridSizer(1, 10, 10);

    m_showNotificationIcon = new wxCheckBox(notifSizer->GetStaticBox(), wxID_ANY, i18n(tr, "show_picotorrent_in_notification_area"));
    m_minimizeNotification = new wxCheckBox(notifSizer->GetStaticBox(), wxID_ANY, i18n(tr, "minimize_to_notification_area"));
    m_closeNotification = new wxCheckBox(notifSizer->GetStaticBox(), wxID_ANY, i18n(tr, "close_to_notification_area"));

    notifGrid->AddGrowableCol(0, 1);
    notifGrid->Add(m_showNotificationIcon);
    notifGrid->Add(m_minimizeNotification);
    notifGrid->Add(m_closeNotification);
    notifSizer->Add(notifGrid, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(uiSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(miscSizer, 0, wxEXPAND);
    sizer->AddSpacer(10);
    sizer->Add(notifSizer, 0, wxEXPAND);
    sizer->AddStretchSpacer();

    for (auto& lang : tr->GetAvailableLanguages())
    {
        int pos = m_language->Append(lang.name, new ClientData<int>(lang.code));

        if (lang.code == m_config->CurrentLanguageId())
        {
            m_language->SetSelection(pos);
        }
    }

    AutoRunKey key;
    if (key.Exists())
    {
        m_autoStart->SetValue(true);
    }

    m_startPosition->Insert(i18n(tr, "normal"), Configuration::WindowState::Normal, new ClientData<Configuration::WindowState>(Configuration::WindowState::Normal));
    m_startPosition->Insert(i18n(tr, "minimized"), Configuration::WindowState::Minimized, new ClientData<Configuration::WindowState>(Configuration::WindowState::Minimized));
    m_startPosition->Insert(i18n(tr, "hidden"), Configuration::WindowState::Hidden, new ClientData<Configuration::WindowState>(Configuration::WindowState::Hidden));
    m_startPosition->Insert(i18n(tr, "maximized"), Configuration::WindowState::Maximized, new ClientData<Configuration::WindowState>(Configuration::WindowState::Maximized));
    m_startPosition->SetSelection(m_config->StartPosition());

    // Notification area
    m_showNotificationIcon->SetValue(m_config->UI()->ShowInNotificationArea());
    m_minimizeNotification->SetValue(m_config->UI()->MinimizeToNotificationArea());
    m_closeNotification->SetValue(m_config->UI()->CloseToNotificationArea());

    this->SetSizerAndFit(sizer);
}

void GeneralPage::ApplyConfiguration()
{
    int langIndex = m_language->GetSelection();
    ClientData<int>* langData = reinterpret_cast<ClientData<int>*>(m_language->GetClientObject(langIndex));

    int startPosIndex = m_startPosition->GetSelection();
    ClientData<Configuration::WindowState>* startPosData = reinterpret_cast<ClientData<Configuration::WindowState>*>(m_startPosition->GetClientObject(startPosIndex));

    m_config->CurrentLanguageId(static_cast<int>(langData->GetValue()));
    m_config->StartPosition(startPosData->GetValue());

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

    m_config->UI()->ShowInNotificationArea(m_showNotificationIcon->GetValue());
    m_config->UI()->MinimizeToNotificationArea(m_minimizeNotification->GetValue());
    m_config->UI()->CloseToNotificationArea(m_closeNotification->GetValue());
}

bool GeneralPage::ValidateConfiguration(wxString& error)
{
    return true;
}
