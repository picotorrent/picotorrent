#include <picotorrent/client/ui/dialogs/magnet_link_dialog.hpp>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>

#include <commctrl.h>
#include <strsafe.h>

using picotorrent::client::ui::dialogs::magnet_link_dialog;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;

magnet_link_dialog::magnet_link_dialog()
    : dialog_base(IDD_ADD_MAGNET_LINK)
{
}

signal_connector<void, void>& magnet_link_dialog::on_add_links()
{
    return on_add_links_;
}

void magnet_link_dialog::close()
{
    PostMessage(handle(), WM_CLOSE, 0, 0);
}

void magnet_link_dialog::disable_actions()
{
    EnableWindow(GetDlgItem(handle(), ID_MAGNET_LINKS_TEXT), FALSE);
    EnableWindow(GetDlgItem(handle(), ID_MAGNET_ADD_LINKS), FALSE);
}

std::vector<std::wstring> magnet_link_dialog::get_links()
{
    std::vector<std::wstring> result;
    std::wstring::size_type pos = 0;
    std::wstring::size_type prev = 0;

    TCHAR links[4096];
    GetDlgItemText(handle(), ID_MAGNET_LINKS_TEXT, links, ARRAYSIZE(links));
    std::wstring l = links;

    while ((pos = l.find(L'\n', prev)) != std::wstring::npos)
    {
        result.push_back(l.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    result.push_back(l.substr(prev));

    return result;
}

void magnet_link_dialog::start_progress()
{
    HWND hProgress = GetDlgItem(handle(), ID_MAGNET_PROGRESS);
    SendMessage(hProgress, PBM_SETMARQUEE, 1, NULL);
}

void magnet_link_dialog::update_status_text(int current, int total)
{
    TCHAR status[1024];
    StringCchPrintf(status, ARRAYSIZE(status), L"(%d/%d)", current, total);
    SetDlgItemText(handle(), ID_MAGNET_CURRENT_STATUS, status);
}

BOOL magnet_link_dialog::on_command(int controlId, WPARAM wParam, LPARAM lParam)
{
    switch (controlId)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(handle(), wParam);
        return TRUE;
    case ID_MAGNET_ADD_LINKS:
        on_add_links_.emit();
        break;
    }

    return FALSE;
}

BOOL magnet_link_dialog::on_init_dialog()
{
    SetWindowText(handle(), TR("add_magnet_link_s"));
    SetDlgItemText(handle(), ID_MAGNET_LINKS_GROUP, TR("magnet_link_s"));
    SetDlgItemText(handle(), ID_MAGNET_ADD_LINKS, TR("add_link_s"));

    return TRUE;
}
