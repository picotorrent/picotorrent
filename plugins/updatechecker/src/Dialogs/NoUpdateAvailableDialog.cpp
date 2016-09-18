#include "NoUpdateAvailableDialog.hpp"

#include <picotorrent/api.hpp>

using Dialogs::NoUpdateAvailableDialog;

NoUpdateAvailableDialog::NoUpdateAvailableDialog(std::shared_ptr<IPicoTorrent> pico)
    : m_pico(pico)
{
}

void NoUpdateAvailableDialog::Show()
{
    // Load translations
    std::wstring main = m_pico->GetTranslator()->Translate("no_update_available");

    TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
    tdf.dwCommonButtons = TDCBF_OK_BUTTON;
    tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW;
    tdf.pszMainIcon = TD_INFORMATION_ICON;
    tdf.pszMainInstruction = main.c_str();
    tdf.pszWindowTitle = TEXT("PicoTorrent");

    m_pico->ShowTaskDialog(&tdf);
}
