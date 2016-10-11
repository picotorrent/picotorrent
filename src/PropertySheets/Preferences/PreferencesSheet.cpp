#include "PreferencesSheet.hpp"

#include "ConnectionPage.hpp"
#include "DownloadsPage.hpp"
#include "GeneralPage.hpp"
#include "PrivacyPage.hpp"

#include "../../Translator.hpp"

using PropertySheets::Preferences::PreferencesSheet;

PreferencesSheet::PreferencesSheet(const std::shared_ptr<UI::NotifyIcon>& notifyIcon)
    :
    m_connection(std::make_unique<ConnectionPage>()),
    m_downloads(std::make_unique<DownloadsPage>()),
    m_general(std::make_unique<GeneralPage>(notifyIcon)),
    m_privacy(std::make_unique<PrivacyPage>())
{
    AddPage(*m_general);
    AddPage(*m_downloads);
    AddPage(*m_connection);
    AddPage(*m_privacy);
}

PreferencesSheet::~PreferencesSheet()
{
}

void PreferencesSheet::OnSheetInitialized()
{
    SetTitle(TRW("preferences"));
}
