#include <picotorrent/client/ui/dialogs/about_dialog.hpp>

#include <picotorrent/core/version_info.hpp>
#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/scaler.hpp>

#include <commctrl.h>
#include <shellapi.h>
#include <strsafe.h>

using picotorrent::core::version_info;
using picotorrent::client::ui::dialogs::about_dialog;
using picotorrent::client::ui::scaler;

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

    SendDlgItemMessage(handle(), ID_PICO_LOGO, STM_SETICON, (WPARAM)hImage, 0);

    int x = ((dlgRc.right - dlgRc.left) - scaler::x(128)) / 2;
    SetWindowPos(GetDlgItem(handle(), ID_PICO_LOGO), HWND_TOPMOST, x, scaler::y(10), -1, -1, SWP_NOSIZE | SWP_NOZORDER);

    SendDlgItemMessage(handle(), ID_PICOTORRENT_V_FORMAT, WM_SETFONT, (WPARAM)title_font_, TRUE);

    TCHAR picoVersion[1024];
    StringCchPrintf(
        picoVersion,
        ARRAYSIZE(picoVersion),
        to_wstring(TR("picotorrent_v_format")).c_str(),
        to_wstring(version_info::current_version()).c_str());

    TCHAR picoBuild[1024];
    StringCchPrintf(
        picoBuild,
        ARRAYSIZE(picoBuild),
        to_wstring(TR("build_info_format")).c_str(),
        to_wstring(version_info::git_branch()).c_str(),
        to_wstring(version_info::git_commit_hash()).c_str());

    // Localize
    set_window_text(TR("about_picotorrent"));
    set_dlg_item_text(ID_PICOTORRENT_V_FORMAT, to_string(picoVersion));
    set_dlg_item_text(ID_BUILD_INFO_FORMAT, to_string(picoBuild));
    set_dlg_item_text(ID_PICOTORRENT_DESCRIPTION, TR("picotorrent_description"));
    set_dlg_item_text(ID_GITHUB_LINK, TR("github_link"));

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

        if ((((LPNMHDR)lParam)->idFrom == ID_GITHUB_LINK) && (item.iLink == 0))
        {
            ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
        }

        break;
    }
    }

    return FALSE;
}
