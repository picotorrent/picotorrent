#include <picotorrent/ui/property_sheets/details/files_page.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/ui/controls/list_view.hpp>
#include <picotorrent/ui/controls/menu.hpp>
#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <shlwapi.h>
#include <strsafe.h>

#define LIST_COLUMN_NAME 1
#define LIST_COLUMN_SIZE 2
#define LIST_COLUMN_PROGRESS 3
#define LIST_COLUMN_PRIORITY 4

using picotorrent::common::signals::signal;
using picotorrent::common::signals::signal_connector;
using picotorrent::common::to_wstring;
using picotorrent::core::torrent;
using picotorrent::ui::controls::list_view;
using picotorrent::ui::controls::menu;
using picotorrent::ui::property_sheets::details::files_page;
using picotorrent::ui::scaler;

struct files_page::file_item
{
    std::wstring name;
    uint64_t size;
    float progress;
    int priority;
};

files_page::files_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_FILES);
    set_title_id(IDS_DETAILS_FILES_TITLE);
}

files_page::~files_page()
{
}

void files_page::add_file(const std::wstring &name, uint64_t size, float progress, int priority)
{
    file_item item{ name,size,progress,priority };
    items_.push_back(item);

    files_->set_item_count((int)items_.size());
}

signal_connector<void, const std::pair<int, int>&>& files_page::on_set_file_priority()
{
    return on_set_file_prio_;
}

void files_page::refresh()
{
    files_->refresh();
}

void files_page::update_file_progress(int index, float progress)
{
    file_item &item = items_[index];
    item.progress = progress;
}

void files_page::on_init_dialog()
{
    HWND hList = GetDlgItem(handle(), ID_DETAILS_FILES_LIST);
    files_ = std::make_unique<list_view>(hList);

    files_->add_column(LIST_COLUMN_NAME,     L"Name",     scaler::x(220));
    files_->add_column(LIST_COLUMN_SIZE,     L"Size",     scaler::x(80),  list_view::number);
    files_->add_column(LIST_COLUMN_PROGRESS, L"Progress", scaler::x(120), list_view::progress);
    files_->add_column(LIST_COLUMN_PRIORITY, L"Priority", scaler::x(80));
    files_->on_display().connect(std::bind(&files_page::on_list_display, this, std::placeholders::_1));
    files_->on_item_context_menu().connect(std::bind(&files_page::on_list_item_context_menu, this, std::placeholders::_1));
    files_->on_progress().connect(std::bind(&files_page::on_list_progress, this, std::placeholders::_1));
}

std::wstring files_page::on_list_display(const std::pair<int, int> &p)
{
    file_item &item = items_[p.second];

    switch (p.first)
    {
    case LIST_COLUMN_NAME:
        return item.name;
    case LIST_COLUMN_SIZE:
    {
        TCHAR size_str[100];
        StrFormatByteSize64(
            item.size,
            size_str,
            ARRAYSIZE(size_str));

        return size_str;
    }
    case LIST_COLUMN_PRIORITY:
    {
        switch (item.priority)
        {
        case torrent::do_not_download:
            return L"Do not download";
        case torrent::normal:
            return L"Normal";
        case torrent::high:
            return L"High";
        case torrent::maximum:
            return L"Maximum";
        }
        
        return L"Unknown priority";
    }
    default:
        return L"<unknown>";
    }
}

void files_page::on_list_item_context_menu(const std::vector<int> &indices)
{
    if (indices.empty())
    {
        return;
    }

    ui::controls::menu menu(IDR_TORRENT_FILE_MENU);
    ui::controls::menu sub = menu.get_sub_menu(0);

    // If only one file is selected, check that files priority
    if (indices.size() == 1)
    {
        int prio = items_[indices[0]].priority;

        switch (prio)
        {
        case torrent::do_not_download:
            sub.check_item(TORRENT_FILE_PRIO_SKIP);
            break;
        case torrent::normal:
            sub.check_item(TORRENT_FILE_PRIO_NORMAL);
            break;
        case torrent::high:
            sub.check_item(TORRENT_FILE_PRIO_HIGH);
            break;
        case torrent::maximum:
            sub.check_item(TORRENT_FILE_PRIO_MAX);
            break;
        }
    }

    POINT p;
    GetCursorPos(&p);

    int res = sub.show(handle(), p);

    for (int i : indices)
    {
        switch (res)
        {
        case TORRENT_FILE_PRIO_SKIP:
            on_set_file_prio_.emit({ i, torrent::do_not_download });
            items_[i].priority = torrent::do_not_download;
            break;
        case TORRENT_FILE_PRIO_NORMAL:
            on_set_file_prio_.emit({ i, torrent::normal });
            items_[i].priority = torrent::normal;
            break;
        case TORRENT_FILE_PRIO_HIGH:
            on_set_file_prio_.emit({ i, torrent::high });
            items_[i].priority = torrent::high;
            break;
        case TORRENT_FILE_PRIO_MAX:
            on_set_file_prio_.emit({ i, torrent::maximum });
            items_[i].priority = torrent::maximum;
            break;
        }
    }

    files_->refresh();
}

float files_page::on_list_progress(const std::pair<int, int> &p)
{
    file_item &item = items_[p.second];

    switch (p.first)
    {
    case LIST_COLUMN_PROGRESS:
        return item.progress;
    }

    return -1;
}
