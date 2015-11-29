#include <picotorrent/ui/dialogs/about_dialog.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/common/version_info.hpp>
#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <commctrl.h>
#include <shellapi.h>
#include <strsafe.h>

using picotorrent::common::to_wstring;
using picotorrent::common::version_info;
using picotorrent::ui::dialogs::about_dialog;
using picotorrent::ui::scaler;

about_dialog::about_dialog()
    : dialog_base(IDD_ABOUT),
    title_font_(CreateFont(scaler::y(24), 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, L"MS Shell Dlg"))
{
}

about_dialog::~about_dialog()
{
    DeleteObject(title_font_);
}

BOOL about_dialog::on_command(int id, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case IDCANCEL:
    case IDOK:
        EndDialog(handle(), wParam);
        return TRUE;
    }

    return FALSE;
}

BOOL about_dialog::on_init_dialog()
{
    RECT dlgRc;
    GetClientRect(handle(), &dlgRc);

    HANDLE hImage = LoadImage(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDI_APPICON),
        IMAGE_ICON,
        scaler::x(128),
        scaler::y(128),
        LR_DEFAULTCOLOR);

    SendDlgItemMessage(handle(), 4654, STM_SETICON, (WPARAM)hImage, 0);

    int x = ((dlgRc.right - dlgRc.left) - scaler::x(128)) / 2;
    SetWindowPos(GetDlgItem(handle(), 4654), HWND_TOPMOST, x, scaler::y(10), -1, -1, SWP_NOSIZE | SWP_NOZORDER);

    SendDlgItemMessage(handle(), 1011, WM_SETFONT, (WPARAM)title_font_, TRUE);

    TCHAR picoVersion[1024];
    StringCchPrintf(
        picoVersion,
        ARRAYSIZE(picoVersion),
        L"PicoTorrent v%s",
        to_wstring(version_info::current_version()).c_str());

    SetWindowText(GetDlgItem(handle(), 1011), picoVersion);

    TCHAR picoBuild[1024];
    StringCchPrintf(
        picoBuild,
        ARRAYSIZE(picoBuild),
        L"Branch: %s\nSHA1: %s",
        to_wstring(version_info::git_branch()).c_str(),
        to_wstring(version_info::git_commit_hash()).c_str());

    SetWindowText(GetDlgItem(handle(), 1012), picoBuild);

    SetWindowText(GetDlgItem(handle(), 1013),
        L"<a href=\"https://github.com/picotorrent/picotorrent\">PicoTorrent on Github</a>");

    return TRUE;
}

BOOL about_dialog::on_notify(LPARAM lParam)
{
    switch (((NMHDR *)lParam)->code)
    {
    case NM_CLICK:
    case NM_RETURN:
    {
        PNMLINK pNMLink = (PNMLINK)lParam;
        LITEM item = pNMLink->item;

        if ((((LPNMHDR)lParam)->idFrom == 1013) && (item.iLink == 0))
        {
            ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
        }

        break;
    }
    }

    return FALSE;
}
