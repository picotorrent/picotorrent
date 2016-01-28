#include <picotorrent/ui/dialogs/add_torrent_dialog.hpp>

#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/controls/list_view.hpp>
#include <commctrl.h>
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

void add_torrent_dialog::add_torrent_file(const std::wstring &name, const std::wstring &friendly_size, const std::wstring &priority)
{
    int items = files_->get_item_count();

    files_->insert_item(items, name);
    files_->set_item(items, 1, friendly_size);
    files_->set_item(items, 2, priority);
}

void add_torrent_dialog::clear_torrent_files()
{
    files_->clear();
}

void add_torrent_dialog::disable_files()
{
    EnableWindow(files_->handle(), FALSE);
}

void add_torrent_dialog::enable_files()
{
    EnableWindow(files_->handle(), TRUE);
}

int add_torrent_dialog::get_selected_torrent()
{
    return ComboBox_GetCurSel(combo_);
}

void add_torrent_dialog::set_file_priority(int index, const std::wstring &prio)
{
    files_->set_item(index, 2, prio);
}

void add_torrent_dialog::set_init_callback(const std::function<void()> &callback)
{
    init_cb_ = callback;
}

void add_torrent_dialog::set_change_callback(const std::function<void(int)> &callback)
{
    change_cb_ = callback;
}

void add_torrent_dialog::set_edit_save_path_callback(const std::function<void()> &callback)
{
    save_path_cb_ = callback;
}

void add_torrent_dialog::set_file_context_menu_callback(const std::function<void(const std::vector<int> &files)> &callback)
{
    files_context_cb_ = callback;
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
            if (change_cb_)
            {
                int index = ComboBox_GetCurSel(combo_);
                change_cb_(index);
            }
            break;
        }
        }
        break;
    }

    case 5005:
    {
        if (save_path_cb_)
        {
            save_path_cb_();
        }
        break;
    }
    }

    return FALSE;
}   

BOOL add_torrent_dialog::on_init_dialog()
{
    combo_ = GetDlgItem(handle(), 5001);
    size_ = GetDlgItem(handle(), 5002);
    save_path_ = GetDlgItem(handle(), 5003);
    files_ = std::make_shared<controls::list_view>(GetDlgItem(handle(), 5004));

    // Set up the files list view
    // files_->add_column(L"Name", 270, LVCFMT_LEFT);
    // files_->add_column(L"Size", 80, LVCFMT_RIGHT);
    // files_->add_column(L"Priority", 120, LVCFMT_LEFT);
    // files_->set_extended_style(LVS_EX_FULLROWSELECT);

    if (init_cb_)
    {
        init_cb_();
    }

    return TRUE;
}

BOOL add_torrent_dialog::on_notify(LPARAM lParam)
{
    LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);

    switch (nmhdr->code)
    {
    case NM_RCLICK:
    {
        if (nmhdr->hwndFrom != files_->handle())
        {
            break;
        }

        if (!files_context_cb_)
        {
            break;
        }

        std::vector<int> selectedFiles = files_->get_selected_items();
        files_context_cb_(selectedFiles);
        break;
    }
    }

    return FALSE;
}
