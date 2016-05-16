#include <picotorrent/client/ui/dialogs/dialog_base.hpp>

#include <picotorrent/client/string_operations.hpp>

using picotorrent::client::to_string;
using picotorrent::client::to_wstring;
using picotorrent::client::ui::dialogs::dialog_base;

dialog_base::dialog_base(int id)
    : id_(id),
    handle_(NULL)
{
}

dialog_base::~dialog_base()
{
}

void dialog_base::close()
{
    PostMessage(handle(), WM_CLOSE, 0, 0);
}

HWND dialog_base::handle()
{
    return handle_;
}

std::string dialog_base::get_dlg_item_text(int controlId)
{
    HWND hItem = GetDlgItem(handle(), controlId);

    int length = GetWindowTextLength(hItem);
    std::vector<wchar_t> t(length + 1);
    GetWindowText(hItem, &t[0], (int)t.size());

    return to_string(&t[0]);
}

bool dialog_base::is_dlg_button_checked(int controlId)
{
    return IsDlgButtonChecked(handle(), controlId) == BST_CHECKED;
}

void dialog_base::set_dlg_button_checked(int controlId, bool checked)
{
    CheckDlgButton(handle(), controlId, checked ? BST_CHECKED : BST_UNCHECKED);
}

void dialog_base::set_dlg_item_text(int controlId, const std::string &text)
{
    HWND hItem = GetDlgItem(handle(), controlId);
    SetWindowText(hItem, to_wstring(text).c_str());
}

void dialog_base::set_window_text(const std::string &text)
{
    SetWindowText(handle(), to_wstring(text).c_str());
}

BOOL dialog_base::on_command(int, WPARAM, LPARAM)
{
    return FALSE;
}

BOOL dialog_base::on_init_dialog()
{
    return TRUE;
}

BOOL dialog_base::on_notify(LPARAM)
{
    return FALSE;
}

int dialog_base::show_modal(HWND parent)
{
    INT_PTR result = DialogBoxParam(
        NULL,
        MAKEINTRESOURCE(id_),
        parent,
        &dialog_base::dlg_proc_proxy,
        (LPARAM)this);

    return (int)result;
}

INT_PTR dialog_base::dlg_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        return on_command(LOWORD(wParam), wParam, lParam);
    }

    case WM_INITDIALOG:
    {
        handle_ = hwndDlg;

        // Center dialog on parent
        HWND hParent = NULL;

        if ((hParent = GetParent(hwndDlg)) == NULL)
        {
            hParent = GetDesktopWindow();
        }

        RECT rcOwner;
        RECT rcDlg;
        RECT rc;

        GetWindowRect(hParent, &rcOwner);
        GetWindowRect(hwndDlg, &rcDlg);
        CopyRect(&rc, &rcOwner);

        OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
        OffsetRect(&rc, -rc.left, -rc.top);
        OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

        SetWindowPos(hwndDlg,
            HWND_TOP,
            rcOwner.left + (rc.right / 2),
            rcOwner.top + 20,
            0, 0,
            SWP_NOSIZE);

        return on_init_dialog();
    }

    case WM_NOTIFY:
    {
        return on_notify(lParam);
    }
    }

    return FALSE;
}

INT_PTR dialog_base::dlg_proc_proxy(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        SetWindowLongPtr(hwndDlg, DWLP_USER, lParam);
    }

    dialog_base* dlg = reinterpret_cast<dialog_base*>(GetWindowLongPtr(hwndDlg, DWLP_USER));

    if (dlg != nullptr)
    {
        return dlg->dlg_proc(hwndDlg, uMsg, wParam, lParam);
    }

    return FALSE;
}
