#include <picotorrent/ui/about_dialog.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/common/version_info.hpp>
#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <commctrl.h>
#include <shellapi.h>
#include <strsafe.h>

using picotorrent::common::to_wstring;
using picotorrent::common::version_info;
using picotorrent::ui::about_dialog;
using picotorrent::ui::scaler;

about_dialog::about_dialog(HWND parent)
    : parent_(parent),
    handle_(NULL),
    title_font_(CreateFont(scaler::y(24), 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, L"MS Shell Dlg"))
{
}

about_dialog::~about_dialog()
{
    DeleteObject(title_font_);
}

int about_dialog::show_modal()
{
    INT_PTR result = DialogBoxParam(
        NULL,
        MAKEINTRESOURCE(IDD_ABOUT),
        parent_,
        &about_dialog::dlg_proc_proxy,
        (LPARAM)this);

    return (int)result;
}

INT_PTR about_dialog::dlg_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
        case IDOK:
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
        break;
    }
    case WM_INITDIALOG:
    {
        handle_ = hwndDlg;

        RECT dlgRc;
        GetClientRect(handle_, &dlgRc);

        HANDLE hImage = LoadImage(
            GetModuleHandle(NULL),
            MAKEINTRESOURCE(IDI_APPICON),
            IMAGE_ICON,
            scaler::x(128),
            scaler::y(128),
            LR_DEFAULTCOLOR);

        SendDlgItemMessage(handle_, 4654, STM_SETICON, (WPARAM)hImage, 0);

        int x = ((dlgRc.right - dlgRc.left) - scaler::x(128)) / 2;
        SetWindowPos(GetDlgItem(handle_, 4654), HWND_TOPMOST, x, scaler::y(10), -1, -1, SWP_NOSIZE | SWP_NOZORDER);

        SendDlgItemMessage(handle_, 1011, WM_SETFONT, (WPARAM)title_font_, TRUE);

        TCHAR picoVersion[1024];
        StringCchPrintf(
            picoVersion,
            ARRAYSIZE(picoVersion),
            L"PicoTorrent v%s",
            to_wstring(version_info::current_version()).c_str());

        SetWindowText(GetDlgItem(handle_, 1011), picoVersion);

        TCHAR picoBuild[1024];
        StringCchPrintf(
            picoBuild,
            ARRAYSIZE(picoBuild),
            L"Branch: %s\nSHA1: %s",
            to_wstring(version_info::git_branch()).c_str(),
            to_wstring(version_info::git_commit_hash()).c_str());

        SetWindowText(GetDlgItem(handle_, 1012), picoBuild);

        SetWindowText(GetDlgItem(handle_, 1013),
            L"<a href=\"https://github.com/picotorrent/picotorrent\">PicoTorrent on Github</a>");

        return TRUE;
    }
    case WM_NOTIFY:
    {
        switch (((NMHDR *)lParam)->code)
        {
        case NM_CLICK:
        case NM_RETURN:
        {
            PNMLINK pNMLink = (PNMLINK)lParam;
            LITEM   item = pNMLink->item;

            if ((((LPNMHDR)lParam)->idFrom == 1013) && (item.iLink == 0))
            {
                ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
            }
        }
        }
        break;
    }
    }

    return false;
}

INT_PTR about_dialog::dlg_proc_proxy(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        SetWindowLongPtr(hwndDlg, DWLP_USER, lParam);
    }

    about_dialog* pWnd = reinterpret_cast<about_dialog*>(GetWindowLongPtr(hwndDlg, DWLP_USER));
    return pWnd->dlg_proc(hwndDlg, uMsg, wParam, lParam);
}
