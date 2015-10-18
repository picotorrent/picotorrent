#include <picotorrent/ui/preferences_dialog.hpp>

#include <picotorrent/ui/resources.hpp>

using picotorrent::ui::preferences_dialog;

preferences_dialog::preferences_dialog(HWND hParent)
    : hParent_(hParent)
{
}

int preferences_dialog::do_modal()
{
    INT_PTR result = DialogBoxParam(
        NULL,
        MAKEINTRESOURCE(IDD_PREFERENCES),
        hParent_,
        &preferences_dialog::dlg_proc_proxy,
        (LPARAM)this);

    return (int)result;
}

std::wstring preferences_dialog::get_text(int controlId)
{
    HWND hItem = GetDlgItem(hWnd_, controlId);

    TCHAR t[1024];
    GetWindowText(hItem, t, ARRAYSIZE(t));

    return t;
}

void preferences_dialog::on_init(const std::function<void(preferences_dialog&)> &callback)
{
    init_cb_ = callback;
}

void preferences_dialog::on_ok(const std::function<void(preferences_dialog&)> &callback)
{
    ok_cb_ = callback;
}

void preferences_dialog::set_text(int controlId, const std::wstring &text)
{
    HWND hItem = GetDlgItem(hWnd_, controlId);
    SetWindowText(hItem, text.c_str());
}

INT_PTR preferences_dialog::dlg_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
        case IDOK:
        {
            if (LOWORD(wParam) == IDOK && ok_cb_)
            {
                ok_cb_(*this);
            }

            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
        }

        break;
    }

    case WM_INITDIALOG:
    {
        hWnd_ = hwndDlg;

        if (init_cb_)
        {
            init_cb_(*this);
        }

        return TRUE;
    }
    }

    return FALSE;
}

INT_PTR preferences_dialog::dlg_proc_proxy(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        SetWindowLongPtr(hwndDlg, DWLP_USER, lParam);
    }

    preferences_dialog* pWnd = reinterpret_cast<preferences_dialog*>(GetWindowLongPtr(hwndDlg, DWLP_USER));
    return pWnd->dlg_proc(hwndDlg, uMsg, wParam, lParam);
}
