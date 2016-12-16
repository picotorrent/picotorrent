#include "UpdateAvailableDialog.hpp"

#include <windows.h>
#include <shellapi.h>
#include <strsafe.h>

#include <picotorrent/api.hpp>
#include <picotorrent/utils.hpp>

#include "../Config/UpdateCheckerConfig.hpp"

#define BTN_SHOW 1000

using Dialogs::UpdateAvailableDialog;

UpdateAvailableDialog::UpdateAvailableDialog(std::shared_ptr<IPicoTorrent> pico)
    : m_pico(pico)
{
}

void UpdateAvailableDialog::Show(std::wstring const& version, std::wstring const& url)
{
    // Load translations
    std::wstring content = m_pico->GetTranslator()->Translate("new_version_available");
    std::wstring main = m_pico->GetTranslator()->Translate("picotorrent_v_available");
    std::wstring verification = m_pico->GetTranslator()->Translate("ignore_update");
    TCHAR mainFormatted[1024];
    StringCchPrintf(mainFormatted, ARRAYSIZE(mainFormatted), main.c_str(), version.c_str());
    std::wstring show = m_pico->GetTranslator()->Translate("show_on_github");

    const TASKDIALOG_BUTTON pButtons[] =
    {
        { BTN_SHOW, show.c_str() },
    };

    TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
    tdf.cButtons = ARRAYSIZE(pButtons);
    tdf.dwCommonButtons = TDCBF_CLOSE_BUTTON;
    tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW | TDF_USE_COMMAND_LINKS;
    tdf.pButtons = pButtons;
    tdf.pszMainIcon = TD_INFORMATION_ICON;
    tdf.pszMainInstruction = mainFormatted;
    tdf.pszVerificationText = verification.c_str();
    tdf.pszWindowTitle = TEXT("PicoTorrent");

    auto result = m_pico->ShowTaskDialog(&tdf);

    if (result->button == BTN_SHOW)
    {
        ShellExecute(
            NULL,
            TEXT("open"),
            url.c_str(),
            NULL,
            NULL,
            SW_SHOWNORMAL);
    }

    if (result->verificationChecked)
    {
        Config::UpdateCheckerConfig cfg(m_pico->GetConfiguration());
        cfg.SetIgnoredVersion(TS(version));
    }
}
