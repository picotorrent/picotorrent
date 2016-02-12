#include <picotorrent/ui/property_sheets/details/files_page.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/i18n/translator.hpp>
#include <picotorrent/ui/controls/list_view.hpp>
#include <picotorrent/ui/controls/menu.hpp>
#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <shellapi.h>
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
    : images_(NULL)
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_FILES);
    set_title(TR("files"));

    images_ = ImageList_Create(
        scaler::x(16),
        scaler::y(16),
        ILC_COLOR,
        10,
        10);
}

files_page::~files_page()
{
}

void files_page::add_file(const std::wstring &name, uint64_t size, float progress, int priority)
{
    file_item item{ name,size,progress,priority };
    items_.push_back(item);

    files_->set_item_count((int)items_.size());

    // Add file extension image to image list
    SHFILEINFO shInfo = { 0 };
    SHGetFileInfo(name.c_str(),
        FILE_ATTRIBUTE_NORMAL,
        &shInfo,
        sizeof(SHFILEINFO),
        SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON);

    int i = ImageList_AddIcon(images_, shInfo.hIcon);
    icon_map_.insert({ name, i });
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

    files_->add_column(LIST_COLUMN_NAME,     TR("name"),     scaler::x(220));
    files_->add_column(LIST_COLUMN_SIZE,     TR("size"),     scaler::x(80),  list_view::number);
    files_->add_column(LIST_COLUMN_PROGRESS, TR("progress"), scaler::x(120), list_view::progress);
    files_->add_column(LIST_COLUMN_PRIORITY, TR("priority"), scaler::x(80));

    files_->on_display().connect(std::bind(&files_page::on_list_display, this, std::placeholders::_1));
    files_->on_item_context_menu().connect(std::bind(&files_page::on_list_item_context_menu, this, std::placeholders::_1));
    files_->on_item_image().connect(std::bind(&files_page::on_list_item_image, this, std::placeholders::_1));
    files_->on_progress().connect(std::bind(&files_page::on_list_progress, this, std::placeholders::_1));

    files_->set_image_list(images_);
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
            return TR("do_not_download");
        case torrent::normal:
            return TR("normal");
        case torrent::high:
            return TR("high");
        case torrent::maximum:
            return TR("maximum");
        }

        return L"<unknown priority>";
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

    HMENU prioMenu = CreateMenu();
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_MAX, TR("maximum"));
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_HIGH, TR("high"));
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_NORMAL, TR("normal"));
    AppendMenu(prioMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_SKIP, TR("do_not_download"));

    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_POPUP, (UINT_PTR)prioMenu, TR("priority"));

    // If only one file is selected, check that files priority
    if (indices.size() == 1)
    {
        int prio = items_[indices[0]].priority;

        switch (prio)
        {
        case torrent::do_not_download:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_SKIP, MF_BYCOMMAND | MF_CHECKED);
            break;
        case torrent::normal:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_NORMAL, MF_BYCOMMAND | MF_CHECKED);
            break;
        case torrent::high:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_HIGH, MF_BYCOMMAND | MF_CHECKED);
            break;
        case torrent::maximum:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_MAX, MF_BYCOMMAND | MF_CHECKED);
            break;
        }
    }

    POINT p;
    GetCursorPos(&p);

    int res = TrackPopupMenu(
        menu,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        0,
        handle(),
        NULL);

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

int files_page::on_list_item_image(const std::pair<int, int> &p)
{
    file_item &item = items_[p.second];
    return icon_map_.at(item.name);
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
