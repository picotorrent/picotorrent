#include "GeneralPage.hpp"

#include <strsafe.h>

#include "../../resources.h"
#include "../../Configuration.hpp"
#include "../../Translator.hpp"
#include "../../UI/NotifyIcon.hpp"

using PropertySheets::Preferences::GeneralPage;

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

GeneralPage::GeneralPage(const std::shared_ptr<UI::NotifyIcon>& notifyIcon)
    : m_notifyIcon(notifyIcon)
{
    m_title = TRW("general");
    SetTitle(m_title.c_str());
}

BOOL GeneralPage::OnApply()
{
    Configuration& cfg = Configuration::GetInstance();
    cfg.SetCurrentLanguageId((int)m_languages.GetItemData(m_languages.GetCurSel()));
    cfg.SetStartupPosition((Configuration::StartupPosition)m_startPosition.GetItemData(m_startPosition.GetCurSel()));

    AutoRunKey key;
    bool isChecked = m_autoStart.GetCheck() == BST_CHECKED;

    if (key.Exists() && !isChecked)
    {
        key.Delete();
    }
    
    if (!key.Exists() && isChecked)
    {
        key.Create();
    }

    cfg.UI()->SetShowInNotificationArea(m_showNotificationIcon.IsChecked());
    cfg.UI()->SetCloseToNotificationArea(m_closeToTray.IsChecked());
    cfg.UI()->SetMinimizeToNotificationArea(m_minimizeToTray.IsChecked());

    if (cfg.UI()->GetShowInNotificationArea())
    {
        m_notifyIcon->Show();
    }
    else
    {
        m_notifyIcon->Hide();
    }

    return TRUE;
}

void GeneralPage::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    switch (nID)
    {
    case ID_LANGUAGE:
    case ID_START_POSITION:
        if (uNotifyCode == CBN_SELENDOK)
        {
            SetModified();
        }
        break;
    case ID_AUTOSTART_PICO:
    {
        bool isChecked = m_autoStart.GetCheck() == BST_CHECKED;
        m_autoStart.SetCheck(isChecked ? BST_UNCHECKED : BST_CHECKED);

        SetModified();
        break;
    }
    case ID_SHOW_IN_NOTIFICATION_AREA:
    case ID_CLOSE_TO_NOTIFICATION_AREA:
    case ID_MINIMIZE_TO_NOTIFICATION_AREA:
        UpdateNotificationState(m_showNotificationIcon.IsChecked());
        SetModified();
        break;
    }
}

BOOL GeneralPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    // Translate
    SetDlgItemText(ID_UI_GROUP, TRW("user_interface"));
    SetDlgItemText(ID_LANGUAGE_TEXT, TRW("language"));
    SetDlgItemText(ID_MISC_GROUP, TRW("miscellaneous"));
    SetDlgItemText(ID_AUTOSTART_PICO, TRW("start_with_windows"));
    SetDlgItemText(ID_START_POSITION_TEXT, TRW("start_position"));

    // Set up controls
    m_languages = GetDlgItem(ID_LANGUAGE);
    m_startPosition = GetDlgItem(ID_START_POSITION);
    m_autoStart = GetDlgItem(ID_AUTOSTART_PICO);
    m_showNotificationIcon = GetDlgItem(ID_SHOW_IN_NOTIFICATION_AREA);
    m_minimizeToTray = GetDlgItem(ID_MINIMIZE_TO_NOTIFICATION_AREA);
    m_closeToTray = GetDlgItem(ID_CLOSE_TO_NOTIFICATION_AREA);

    for (const Translator::Language &lang : Translator::GetInstance().GetAvailableLanguages())
    {
        int index = m_languages.AddString(ToWideString(lang.name).c_str());
        m_languages.SetItemData(index, lang.code);
    }

    m_startPosition.SetItemData(m_startPosition.AddString(TRW("normal")), Configuration::StartupPosition::Normal);
    m_startPosition.SetItemData(m_startPosition.AddString(TRW("minimized")), Configuration::StartupPosition::Minimized);
    m_startPosition.SetItemData(m_startPosition.AddString(TRW("hidden")), Configuration::StartupPosition::Hidden);
    m_startPosition.SetItemData(m_startPosition.AddString(TRW("maximized")), Configuration::StartupPosition::Maximized);

    Configuration& cfg = Configuration::GetInstance();
    SelectComboBoxItemWithData(m_languages, cfg.GetCurrentLanguageId());
    SelectComboBoxItemWithData(m_startPosition, cfg.GetStartupPosition());

    AutoRunKey key;
    if (key.Exists())
    {
        m_autoStart.SetCheck(BST_CHECKED);
    }

    m_showNotificationIcon.SetCheck(cfg.UI()->GetShowInNotificationArea() ? BST_CHECKED : BST_UNCHECKED);
    m_closeToTray.SetCheck(cfg.UI()->GetCloseToNotificationArea() ? BST_CHECKED : BST_UNCHECKED);
    m_minimizeToTray.SetCheck(cfg.UI()->GetMinimizeToNotificationArea() ? BST_CHECKED : BST_UNCHECKED);

    return TRUE;
}

void GeneralPage::SelectComboBoxItemWithData(CComboBox& cb, int data)
{
    for (int i = 0; i < cb.GetCount(); i++)
    {
        LRESULT d = cb.GetItemData(i);

        if (d == data)
        {
            cb.SetCurSel(i);
            break;
        }
    }
}

void GeneralPage::UpdateNotificationState(bool checked)
{
    m_closeToTray.EnableWindow(checked ? TRUE : FALSE);
    m_minimizeToTray.EnableWindow(checked ? TRUE : FALSE);
}
