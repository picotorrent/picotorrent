#include "ApplicationCloseController.hpp"

#include <commctrl.h>

#include "../Configuration.hpp"
#include "../Translator.hpp"

#define BTN_CLOSE 101
#define BTN_MINIMIZE 102

using Controllers::ApplicationCloseController;

ApplicationCloseController::ApplicationCloseController(HWND hWndMainWindow)
    : m_hWndMainWindow(hWndMainWindow)
{
}

bool ApplicationCloseController::Execute()
{
    Configuration& cfg = Configuration::GetInstance();

    switch (cfg.GetCloseAction())
    {
    case Configuration::CloseAction::Close:
        return true;
    case Configuration::CloseAction::Minimize:
        ShowWindow(m_hWndMainWindow, SW_HIDE);
        break;
    case Configuration::CloseAction::Prompt:
        if (Prompt()) { return true; }
        break;
    }

    return false;
}

bool ApplicationCloseController::Prompt()
{
    std::wstring btn_close = TRW("close_picotorrent");
    std::wstring btn_min = TRW("minimize_to_tray");
    std::wstring content = TRW("confirm_close_description");
    std::wstring main = TRW("confirm_close");
    std::wstring verification = TRW("remember_choice");

    const TASKDIALOG_BUTTON pButtons[] =
    {
        { BTN_CLOSE, btn_close.c_str() },
        { BTN_MINIMIZE, btn_min.c_str() }
    };

    TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
    tdf.cButtons = ARRAYSIZE(pButtons);
    tdf.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW | TDF_USE_COMMAND_LINKS;
    tdf.hwndParent = m_hWndMainWindow;
    tdf.lpCallbackData = reinterpret_cast<LONG_PTR>(this);
    tdf.pButtons = pButtons;
    tdf.pszContent = content.c_str();
    tdf.pszMainIcon = TD_INFORMATION_ICON;
    tdf.pszMainInstruction = main.c_str();
    tdf.pszVerificationText = verification.c_str();
    tdf.pszWindowTitle = TEXT("PicoTorrent");

    int pButton = -1;
    BOOL pfVerificationFlagChecked = FALSE;

    TaskDialogIndirect(&tdf, &pButton, NULL, &pfVerificationFlagChecked);

    if (pfVerificationFlagChecked)
    {
        Configuration& cfg = Configuration::GetInstance();

        switch (pButton)
        {
        case BTN_CLOSE:
            cfg.SetCloseAction(Configuration::CloseAction::Close);
            break;
        case BTN_MINIMIZE:
            cfg.SetCloseAction(Configuration::CloseAction::Minimize);
            break;
        }
    }

    switch (pButton)
    {
    case BTN_CLOSE:
        return true;
    case BTN_MINIMIZE:
        ShowWindow(m_hWndMainWindow, SW_HIDE);
        break;
    }

    return false;
}
