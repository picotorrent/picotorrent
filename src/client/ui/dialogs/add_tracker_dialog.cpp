#include <picotorrent/client/ui/dialogs/add_tracker_dialog.hpp>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>

#include <commctrl.h>
#include <strsafe.h>

using picotorrent::client::ui::dialogs::add_tracker_dialog;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;

add_tracker_dialog::add_tracker_dialog()
    : dialog_base(IDD_ADD_TRACKER)
{
}

void add_tracker_dialog::close()
{
    PostMessage(handle(), WM_CLOSE, 0, 0);
}

std::wstring add_tracker_dialog::get_url()
{
    TCHAR url[4096];
    GetDlgItemText(handle(), ID_ADD_TRACKER_URL, url, ARRAYSIZE(url));
    return url_;
}

BOOL add_tracker_dialog::on_command(int controlId, WPARAM wParam, LPARAM lParam)
{
    switch (controlId)
    {
    case IDOK:
        TCHAR url[4096];
        GetDlgItemText(handle(), ID_ADD_TRACKER_URL, url, ARRAYSIZE(url));
        url_ = url;
    case IDCANCEL:
        EndDialog(handle(), wParam);
        return TRUE;
    }

    return FALSE;
}

BOOL add_tracker_dialog::on_init_dialog()
{
    set_window_text(TR("add_tracker"));
    set_dlg_item_text(IDOK, TR("add_tracker"));
    return TRUE;
}
