#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>

using picotorrent::ui::property_sheets::property_sheet_page;

property_sheet_page::property_sheet_page()
    : page_(std::make_unique<PROPSHEETPAGE>())
{
    page_->dwSize = sizeof(PROPSHEETPAGE);
    page_->lParam = (LPARAM)this;
    page_->pfnCallback = NULL;
    page_->pfnDlgProc = &property_sheet_page::dlg_proc_proxy;
}

INT_PTR property_sheet_page::dlg_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        return on_command(hwndDlg, LOWORD(wParam), wParam, lParam);
    }

    case WM_INITDIALOG:
        handle_ = hwndDlg;
        if (init_cb_)
        {
            is_initializing_ = true;
            init_cb_();
            is_initializing_ = false;
        }

        return TRUE;

    case WM_NOTIFY:
        LPNMHDR pnmh = (LPNMHDR)lParam;

        switch (pnmh->code)
        {
        case PSN_KILLACTIVE:
            SetWindowLongPtr(pnmh->hwndFrom, DWLP_MSGRESULT, FALSE);
            return TRUE;

        case PSN_APPLY:
            if (apply_cb_) { apply_cb_(); }
            SetWindowLongPtr(pnmh->hwndFrom, DWLP_MSGRESULT, PSNRET_NOERROR);
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

void property_sheet_page::set_flags(DWORD flags)
{
    page_->dwFlags = flags;
}

void property_sheet_page::set_apply_callback(const std::function<void()> &callback)
{
    apply_cb_ = callback;
}

void property_sheet_page::set_init_callback(const std::function<void()> &callback)
{
    init_cb_ = callback;
}

void property_sheet_page::set_instance(HINSTANCE instance)
{
    page_->hInstance = instance;
}

void property_sheet_page::set_template_id(int id)
{
    page_->pszTemplate = MAKEINTRESOURCE(id);
}

void property_sheet_page::set_title_id(int id)
{
    page_->pszTitle = MAKEINTRESOURCE(id);
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
