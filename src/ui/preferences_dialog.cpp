#include <picotorrent/ui/preferences_dialog.hpp>

#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/ui/open_file_dialog.hpp>
#include <picotorrent/ui/resources.hpp>

namespace fs = picotorrent::filesystem;
using picotorrent::ui::preferences_dialog;

const GUID DLG_BROWSE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x62 } };

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

bool preferences_dialog::get_checked(int controlId)
{
    return IsDlgButtonChecked(hWnd_, controlId);
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

void preferences_dialog::set_checked(int controlId, bool checked)
{
    CheckDlgButton(hWnd_, controlId, checked ? BST_CHECKED : BST_UNCHECKED);
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

        case ID_PREFS_DEFSAVEPATH_BROWSE:
        {
            ui::open_file_dialog dlg;
            dlg.set_guid(DLG_BROWSE);
            dlg.set_folder(get_text(ID_PREFS_DEFSAVEPATH));
            dlg.set_options(dlg.options() | FOS_PICKFOLDERS);
            dlg.set_title(TEXT("Choose save path"));

            dlg.show(hWnd_);

            std::vector<fs::path> paths = dlg.get_paths();

            if (paths.size() > 0)
            {
                set_text(ID_PREFS_DEFSAVEPATH, paths[0].to_string());
            }

            break;
        }
        
        case ID_PREFS_PROMPTFORSAVEPATH:
        {
            set_checked(ID_PREFS_PROMPTFORSAVEPATH, !get_checked(ID_PREFS_PROMPTFORSAVEPATH));
            break;
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
