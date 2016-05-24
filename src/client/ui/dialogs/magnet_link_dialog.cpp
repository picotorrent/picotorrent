#include <picotorrent/client/ui/dialogs/magnet_link_dialog.hpp>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <sstream>

#include <commctrl.h>
#include <strsafe.h>

using picotorrent::client::ui::dialogs::magnet_link_dialog;
using picotorrent::common::to_string;
using picotorrent::common::to_wstring;
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

void magnet_link_dialog::disable_actions()
{
    EnableWindow(GetDlgItem(handle(), ID_MAGNET_LINKS_TEXT), FALSE);
    EnableWindow(GetDlgItem(handle(), ID_MAGNET_ADD_LINKS), FALSE);
}

std::vector<std::string> magnet_link_dialog::get_links()
{
    std::vector<std::string> result;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    std::string l = get_dlg_item_text(ID_MAGNET_LINKS_TEXT);

    while ((pos = l.find('\n', prev)) != std::string::npos)
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
    std::stringstream status_text;
    status_text << "(" << current << "/" << total << ")";
    set_dlg_item_text(ID_MAGNET_CURRENT_STATUS, status_text.str());
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
    set_window_text(TR("add_magnet_link_s"));
    set_dlg_item_text(ID_MAGNET_LINKS_GROUP, TR("magnet_link_s"));
    set_dlg_item_text(ID_MAGNET_ADD_LINKS, TR("add_link_s"));

    return TRUE;
}
