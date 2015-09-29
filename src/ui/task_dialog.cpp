#include <picotorrent/ui/task_dialog.hpp>

#include <windows.h>
#include <commctrl.h>
#include <strsafe.h>

using picotorrent::ui::task_dialog;

task_dialog::~task_dialog()
{
    for (TASKDIALOG_BUTTON &btn : buttons_)
    {
        delete[] btn.pszButtonText;
    }
}

void task_dialog::add_button(int id, const std::wstring &text, const std::function<bool()> &callback)
{
    wchar_t *t = new wchar_t[100];
    StringCchCopy(t, 100, text.c_str());

    TASKDIALOG_BUTTON btn;
    btn.nButtonID = id;
    btn.pszButtonText = t;

    buttons_.push_back(btn);
    callbacks_.insert({ id, callback });
}

void task_dialog::set_content(const std::wstring &text)
{
    content_ = text;
}

void task_dialog::set_main_icon(PCWSTR icon)
{
    icon_ = icon;
}

void task_dialog::set_main_instruction(const std::wstring &text)
{
    instruction_ = text;
}

void task_dialog::set_parent(HWND parent)
{
    parent_ = parent;
}

void task_dialog::set_title(const std::wstring &title)
{
    title_ = title;
}

void task_dialog::show()
{
    TASKDIALOGCONFIG config = { 0 };
    config.cbSize = sizeof(config);
    config.hwndParent = parent_;
    config.pButtons = buttons_.data();
    config.cButtons = buttons_.size();
    config.pszWindowTitle = title_.c_str();
    config.pszMainIcon = icon_;
    config.pszMainInstruction = instruction_.c_str();
    config.pszContent = content_.c_str();
    config.lpCallbackData = (LONG_PTR)this;
    config.pfCallback = (PFTASKDIALOGCALLBACK)&task_dialog::callback;

    config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    config.dwFlags = TDF_USE_COMMAND_LINKS;

    int button = 0;

    TaskDialogIndirect(
        &config,
        &button,
        NULL,
        NULL);
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
    }

    return S_OK;
}
