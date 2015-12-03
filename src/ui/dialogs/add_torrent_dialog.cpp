#include <picotorrent/ui/dialogs/add_torrent_dialog.hpp>

#include <picotorrent/ui/resources.hpp>
#include <windowsx.h>

using picotorrent::ui::dialogs::add_torrent_dialog;

add_torrent_dialog::add_torrent_dialog()
    : dialog_base(IDD_ADD_TORRENT),
    combo_(NULL),
    save_path_(NULL),
    size_(NULL),
    files_(NULL)
{
}

void add_torrent_dialog::add_torrent(const std::wstring &name)
{
    ComboBox_AddString(combo_, name.c_str());
}

void add_torrent_dialog::set_init_callback(const std::function<void()> &callback)
{
    init_cb_ = callback;
}

void add_torrent_dialog::set_save_path(const std::wstring &path)
{
    Edit_SetText(save_path_, path.c_str());
}

void add_torrent_dialog::set_selected_item(int item)
{
    ComboBox_SetCurSel(combo_, item);
}

void add_torrent_dialog::set_size(const std::wstring &friendly_size)
{
    Edit_SetText(size_, friendly_size.c_str());
}

BOOL add_torrent_dialog::on_command(int controlId, WPARAM wParam, LPARAM lParam)
{
    switch (controlId)
    {
    case IDCANCEL:
    case IDOK:
    {
        EndDialog(handle(), wParam);
        return TRUE;
    }

    case 5001:
    {
        switch (HIWORD(wParam))
        {
        case CBN_SELENDOK:
        {
            printf("");
            break;
        }
        }
    }
    }

    return FALSE;
}   

BOOL add_torrent_dialog::on_init_dialog()
{
    combo_ = GetDlgItem(handle(), 5001);
    size_ = GetDlgItem(handle(), 5002);
    save_path_ = GetDlgItem(handle(), 5003);

    if (init_cb_)
    {
        init_cb_();
    }

    return TRUE;
}
