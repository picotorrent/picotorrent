#include <picotorrent/client/ui/task_dialog.hpp>

#include <picotorrent/client/string_operations.hpp>

#include <windows.h>
#include <commctrl.h>
#include <strsafe.h>

using picotorrent::client::ui::task_dialog;

task_dialog::task_dialog()
    : parent_(NULL),
    common_buttons_(TDCBF_CANCEL_BUTTON)
{
}

task_dialog::~task_dialog()
{
    for (TASKDIALOG_BUTTON &btn : buttons_)
    {
        delete[] btn.pszButtonText;
    }
}

void task_dialog::add_button(int id, const std::string &text, const std::function<bool()> &callback)
{
    wchar_t *t = new wchar_t[100];
    StringCchCopy(t, 100, to_wstring(text).c_str());

    TASKDIALOG_BUTTON btn;
    btn.nButtonID = id;
    btn.pszButtonText = t;

    buttons_.push_back(btn);
    callbacks_.insert({ id, callback });
}

bool task_dialog::is_verification_checked()
{
    return verification_checked_;
}

void task_dialog::set_common_buttons(DWORD buttons)
{
    common_buttons_ = buttons;
}

void task_dialog::set_content(const std::string &text)
{
    content_ = to_wstring(text);
}

void task_dialog::set_main_icon(PCWSTR icon)
{
    icon_ = icon;
}

void task_dialog::set_main_instruction(const std::string &text)
{
    instruction_ = to_wstring(text);
}

void task_dialog::set_parent(HWND parent)
{
    parent_ = parent;
}

void task_dialog::set_title(const std::string &title)
{
    title_ = to_wstring(title);
}

void task_dialog::set_verification_text(const std::string &text)
{
    verification_ = to_wstring(text);
}

int task_dialog::show()
{
    TASKDIALOGCONFIG config = { 0 };
    config.cbSize = sizeof(config);
    config.hwndParent = parent_;
    config.pButtons = buttons_.data();
    config.cButtons = (UINT)buttons_.size();
    config.pszWindowTitle = title_.c_str();
    config.pszMainIcon = icon_;
    config.pszMainInstruction = instruction_.c_str();
    config.pszContent = content_.c_str();
    config.lpCallbackData = (LONG_PTR)this;
    config.pfCallback = (PFTASKDIALOGCALLBACK)&task_dialog::callback;
    config.pszVerificationText = verification_.c_str();
    config.dwCommonButtons = common_buttons_;

    if (buttons_.size() > 0)
    {
        config.dwFlags = TDF_USE_COMMAND_LINKS;
    }

    int button = 0;
    BOOL verificationFlag;

    TaskDialogIndirect(
        &config,
        &button,
        NULL,
        &verificationFlag);
    
    verification_checked_ = verificationFlag == TRUE ? true : false;
    return button;
}

HRESULT task_dialog::callback(HWND hWnd, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData)
{
    task_dialog *dlg = reinterpret_cast<task_dialog*>(dwRefData);
    
    switch (uNotification)
    {
    case TDN_BUTTON_CLICKED:
    {
        int id = (int)wParam;

        if (dlg->callbacks_.find(id) != dlg->callbacks_.end())
        {
            if (dlg->callbacks_.find(id)->second())
            {
                return S_FALSE;
            }

            break;
        }

        break;
    }
    case TDN_DIALOG_CONSTRUCTED:
    {
        if (dlg->parent_ == NULL)
        {
            break;
        }

        RECT parentRect;
        GetWindowRect(dlg->parent_, &parentRect);

        RECT dlgRect;
        GetWindowRect(hWnd, &dlgRect);

        int parentWidth = parentRect.right - parentRect.left;
        int parentCenter = parentWidth / 2;

        int dlgWidth = dlgRect.right - dlgRect.left;
        int dlgLeft = parentRect.left + (parentCenter - dlgWidth / 2);

        // Center the dialog
        MoveWindow(hWnd, dlgLeft, parentRect.top + 20, dlgWidth, dlgRect.top - dlgRect.bottom, TRUE);
        break;
    }
    }

    return S_OK;
}
