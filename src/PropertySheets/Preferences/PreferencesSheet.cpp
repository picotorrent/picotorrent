#include "PreferencesSheet.hpp"

#include "DownloadsPage.hpp"
#include "GeneralPage.hpp"

#include "../../Translator.hpp"

using PropertySheets::Preferences::PreferencesSheet;

PreferencesSheet::PreferencesSheet()
    :
    m_downloads(std::make_unique<DownloadsPage>()),
    m_general(std::make_unique<GeneralPage>())
{
    AddPage(*m_general);
    AddPage(*m_downloads);
}

PreferencesSheet::~PreferencesSheet()
{
}

void PreferencesSheet::OnSheetInitialized()
{
    SetTitle(TRW("preferences"));
}
