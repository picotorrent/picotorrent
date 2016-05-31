#include <picotorrent/client/ui/dialogs/add_torrent_dialog.hpp>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/controls/list_view.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <commctrl.h>
#include <windowsx.h>
#include <shlwapi.h>

#define LIST_COLUMN_NAME 1
#define LIST_COLUMN_SIZE 2
#define LIST_COLUMN_PRIO 3

using picotorrent::client::ui::dialogs::add_torrent_dialog;
using picotorrent::common::to_string;
using picotorrent::common::to_wstring;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;

struct add_torrent_dialog::file_item
{
    std::string name;
    std::string size;
    std::string priority;
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

void add_torrent_dialog::add_torrent(const std::string &name)
{
    ComboBox_AddString(combo_, to_wstring(name).c_str());
}

void add_torrent_dialog::add_torrent_file(const std::string &name, int64_t size, const std::string &priority)
{
    TCHAR s[1024];
    StrFormatByteSize64(size, s, ARRAYSIZE(s));

    file_item item{ name, to_string(s),priority };
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

signal_connector<void, void>& add_torrent_dialog::on_update_storage_mode()
{
    return on_update_storage_mode_;
}

void add_torrent_dialog::set_file_priority(int index, const std::string &prio)
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

void add_torrent_dialog::set_save_path(const std::string &path)
{
    Edit_SetText(save_path_, to_wstring(path).c_str());
}

void add_torrent_dialog::set_selected_item(int item)
{
    ComboBox_SetCurSel(combo_, item);
}

void add_torrent_dialog::set_size(int64_t size)
{
    TCHAR s[1024];
    StrFormatByteSize64(size, s, ARRAYSIZE(s));
    Edit_SetText(size_, s);
}

void add_torrent_dialog::set_size(const std::string &friendly_size)
{
    Edit_SetText(size_, to_wstring(friendly_size).c_str());
}

bool add_torrent_dialog::use_full_allocation()
{
    return is_dlg_button_checked(ID_ADD_STORAGE_MODE_FULL);
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

    case ID_TORRENT:
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

    case ID_BROWSE:
    {
        if (save_path_cb_)
        {
            save_path_cb_();
        }
        break;
    }

    case ID_ADD_STORAGE_MODE_SPARSE:
    {
        set_dlg_button_checked(ID_ADD_STORAGE_MODE_SPARSE, true);
        set_dlg_button_checked(ID_ADD_STORAGE_MODE_FULL, false);
        on_update_storage_mode_.emit();
        break;
    }

    case ID_ADD_STORAGE_MODE_FULL:
    {
        set_dlg_button_checked(ID_ADD_STORAGE_MODE_SPARSE, false);
        set_dlg_button_checked(ID_ADD_STORAGE_MODE_FULL, true);
        on_update_storage_mode_.emit();
        break;
    }
    }

    return FALSE;
}   

BOOL add_torrent_dialog::on_init_dialog()
{
    combo_ = GetDlgItem(handle(), ID_TORRENT);
    size_ = GetDlgItem(handle(), ID_SIZE);
    save_path_ = GetDlgItem(handle(), ID_SAVE_PATH);

    // Localize
    set_window_text(TR("add_torrent_s"));
    set_dlg_item_text(ID_TORRENT_TEXT, TR("torrent"));
    set_dlg_item_text(ID_SIZE_TEXT, TR("size"));
    set_dlg_item_text(ID_SAVE_PATH_TEXT, TR("save_path"));
    set_dlg_item_text(ID_BROWSE, TR("browse"));
    set_dlg_item_text(ID_STORAGE_GROUP, TR("storage"));
    set_dlg_item_text(IDOK, TR("add_torrent_s"));
    set_dlg_item_text(ID_ADD_STORAGE_MODE_TEXT, TR("storage_mode"));
    set_dlg_item_text(ID_ADD_STORAGE_MODE_SPARSE, TR("sparse"));
    set_dlg_item_text(ID_ADD_STORAGE_MODE_FULL, TR("full"));

    // Check the sparse mode radio button
    CheckDlgButton(handle(), ID_ADD_STORAGE_MODE_SPARSE, BST_CHECKED);

    // Set up the files list view
    files_ = std::make_shared<controls::list_view>(GetDlgItem(handle(), ID_FILES));
    files_->add_column(LIST_COLUMN_NAME, TR("name"), 270);
    files_->add_column(LIST_COLUMN_SIZE, TR("size"), 80, controls::list_view::number);
    files_->add_column(LIST_COLUMN_PRIO, TR("priority"), 120);
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

std::string add_torrent_dialog::on_list_display(const std::pair<int, int> &p)
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
        return "<unknown>";
    }
}

