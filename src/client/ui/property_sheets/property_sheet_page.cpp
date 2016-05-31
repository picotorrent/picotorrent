#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>

#include <picotorrent/client/ui/task_dialog.hpp>
#include <picotorrent/common/string_operations.hpp>

using picotorrent::client::ui::property_sheets::property_sheet_page;
using picotorrent::client::ui::task_dialog;
using picotorrent::common::to_string;
using picotorrent::common::to_wstring;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;

property_sheet_page::property_sheet_page()
    : page_(std::make_unique<PROPSHEETPAGE>())
{
    page_->dwSize = sizeof(PROPSHEETPAGE);
    page_->lParam = (LPARAM)this;
    page_->pfnCallback = NULL;
    page_->pfnDlgProc = &property_sheet_page::dlg_proc_proxy;
}

signal_connector<void, void>& property_sheet_page::on_activate() { return on_activate_; }
signal_connector<void, void>& property_sheet_page::on_apply() { return on_apply_; }
signal_connector<void, void>& property_sheet_page::on_destroy() { return on_destroy_; }
signal_connector<void, void>& property_sheet_page::on_init() { return on_init_; }
signal_connector<bool, void>& property_sheet_page::on_validate() { return on_validate_; }

INT_PTR property_sheet_page::dlg_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        return on_command(hwndDlg, LOWORD(wParam), wParam, lParam);
    }

    case WM_DESTROY:
    {
        on_destroy_.emit();
        break;
    }

    case WM_INITDIALOG:
    {
        handle_ = hwndDlg;

        // Center dialog on parent
        HWND hParent = GetParent(hwndDlg);
        HWND hBase = GetParent(hParent);
        RECT rcOwner;
        RECT rcDlg;
        RECT rc;

        GetWindowRect(hBase, &rcOwner);
        GetWindowRect(hParent, &rcDlg);
        CopyRect(&rc, &rcOwner);

        OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
        OffsetRect(&rc, -rc.left, -rc.top);
        OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

        SetWindowPos(hParent,
            HWND_TOP,
            rcOwner.left + (rc.right / 2),
            rcOwner.top + 20,
            0, 0,
            SWP_NOSIZE);

        is_initializing_ = true;
        on_init_dialog();
        on_init_.emit();
        is_initializing_ = false;

        return TRUE;
    }

    case WM_NOTIFY:
        LPNMHDR pnmh = (LPNMHDR)lParam;
        LRESULT res = FALSE;

        // If the on_notify override decides to handle this message.
        if (on_notify(hwndDlg, pnmh, res))
        {
            SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, res);
            return TRUE;
        }

        switch (pnmh->code)
        {
        case PSN_SETACTIVE:
        {
            on_activate_.emit();
            break;
        }

        case PSN_KILLACTIVE:
        {
            std::vector<bool> ret = on_validate_.emit();
            bool valid = std::all_of(ret.begin(), ret.end(), [](bool v) { return v == true; });
            SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, valid ? FALSE : TRUE);
            return TRUE;
        }

        case PSN_APPLY:
            on_apply_.emit();
            SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, PSNRET_NOERROR);
            return TRUE;
        }

        break;
    }

    return FALSE;
}

HWND property_sheet_page::handle()
{
    return handle_;
}

bool property_sheet_page::is_initializing()
{
    return is_initializing_;
}

std::string property_sheet_page::get_dlg_item_text(int id)
{
    TCHAR text[1024];
    GetDlgItemText(handle(), id, text, ARRAYSIZE(text));
    return to_string(text);
}

void property_sheet_page::set_dlg_item_text(int id, const std::string &text)
{
    SetDlgItemText(handle(), id, to_wstring(text).c_str());
}

void property_sheet_page::set_flags(DWORD flags)
{
    page_->dwFlags = flags;
}

void property_sheet_page::set_instance(HINSTANCE instance)
{
    page_->hInstance = instance;
}

void property_sheet_page::set_template_id(int id)
{
    page_->pszTemplate = MAKEINTRESOURCE(id);
}

void property_sheet_page::set_title(const std::string &title)
{
    title_ = to_wstring(title);
    page_->pszTitle = title_.c_str();
}

void property_sheet_page::show_error_message(const std::string &text)
{
    task_dialog dlg;
    dlg.set_common_buttons(TDCBF_OK_BUTTON);
    dlg.set_content(text);
    dlg.set_main_icon(TD_ERROR_ICON);
    dlg.set_parent(handle());
    dlg.set_title("PicoTorrent");
    dlg.show();
}

INT_PTR property_sheet_page::dlg_proc_proxy(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        PROPSHEETPAGE *p = reinterpret_cast<PROPSHEETPAGE*>(lParam);
        SetWindowLongPtr(hwndDlg, DWLP_USER, p->lParam);
    }

    property_sheet_page* page = reinterpret_cast<property_sheet_page*>(GetWindowLongPtr(hwndDlg, DWLP_USER));

    if (page != nullptr)
    {
        return page->dlg_proc(hwndDlg, uMsg, wParam, lParam);
    }

    return FALSE;
}
