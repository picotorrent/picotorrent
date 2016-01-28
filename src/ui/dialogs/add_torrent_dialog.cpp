#include <picotorrent/ui/dialogs/add_torrent_dialog.hpp>

#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/controls/list_view.hpp>
#include <commctrl.h>
#include <windowsx.h>

#define LIST_COLUMN_NAME 1
#define LIST_COLUMN_SIZE 2
#define LIST_COLUMN_PRIO 3

using picotorrent::ui::dialogs::add_torrent_dialog;

struct add_torrent_dialog::file_item
{
    std::wstring name;
    std::wstring size;
    std::wstring priority;
};

add_torrent_dialog::add_torrent_dialog()
    : dialog_base(IDD_ADD_TORRENT),
    combo_(NULL),
    save_path_(NULL),
    size_(NULL),
    files_(NULL)
{
}

add_torrent_dialog::~add_torrent_dialog()
{
}

void add_torrent_dialog::add_torrent(const std::wstring &name)
{
    ComboBox_AddString(combo_, name.c_str());
}

void add_torrent_dialog::add_torrent_file(const std::wstring &name, const std::wstring &friendly_size, const std::wstring &priority)
{
    file_item item{ name, friendly_size,priority };
    items_.push_back(item);

    files_->set_item_count((int)items_.size());
}

void add_torrent_dialog::clear_torrent_files()
{
    items_.clear();
    files_->set_item_count(0);
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
    file_item &item = items_[index];
    item.priority = prio;

    files_->set_item_count((int)items_.size());
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

    // Set up the files list view
    files_ = std::make_shared<controls::list_view>(GetDlgItem(handle(), 5004));
    files_->add_column(LIST_COLUMN_NAME, L"Name", 270);
    files_->add_column(LIST_COLUMN_SIZE, L"Size", 80, controls::list_view::number);
    files_->add_column(LIST_COLUMN_PRIO, L"Priority", 120);
    files_->on_display().connect(std::bind(&add_torrent_dialog::on_list_display, this, std::placeholders::_1));

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

        std::vector<int> selectedFiles = files_->get_selection();
        files_context_cb_(selectedFiles);
        break;
    }
    }

    return FALSE;
}

std::wstring add_torrent_dialog::on_list_display(const std::pair<int, int> &p)
{
    file_item &item = items_[p.second];

    switch (p.first)
    {
    case LIST_COLUMN_NAME:
        return item.name;
    case LIST_COLUMN_SIZE:
        return item.size;
    case LIST_COLUMN_PRIO:
        return item.priority;
    default:
        return L"<unknown>";
    }
}

