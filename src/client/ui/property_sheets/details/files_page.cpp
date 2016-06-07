#include <picotorrent/client/ui/property_sheets/details/files_page.hpp>

#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/controls/list_view.hpp>
#include <picotorrent/client/ui/controls/menu.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/scaler.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/common/string_operations.hpp>

#include <shellapi.h>
#include <shlwapi.h>
#include <strsafe.h>

#define LIST_COLUMN_NAME 1
#define LIST_COLUMN_SIZE 2
#define LIST_COLUMN_PROGRESS 3
#define LIST_COLUMN_PRIORITY 4

using picotorrent::client::ui::controls::list_view;
using picotorrent::client::ui::controls::menu;
using picotorrent::client::ui::property_sheets::details::files_page;
using picotorrent::client::ui::scaler;
using picotorrent::common::to_string;
using picotorrent::common::to_wstring;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;
using picotorrent::core::torrent;

struct files_page::file_item
{
    std::string name;
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

void files_page::add_file(const std::string &name, uint64_t size, float progress, int priority)
{
    file_item item{ name,size,progress,priority };
    items_.push_back(item);

    files_->set_item_count((int)items_.size());

    // Add file extension image to image list
    SHFILEINFO shInfo = { 0 };
    SHGetFileInfo(to_wstring(name).c_str(),
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

    files_->load_state("torrent_files_list");
    on_destroy().connect([this]()
    {
        files_->save_state("torrent_files_list");
    });

    files_->on_display().connect(std::bind(&files_page::on_list_display, this, std::placeholders::_1));
    files_->on_item_context_menu().connect(std::bind(&files_page::on_list_item_context_menu, this, std::placeholders::_1));
    files_->on_item_image().connect(std::bind(&files_page::on_list_item_image, this, std::placeholders::_1));
    files_->on_progress().connect(std::bind(&files_page::on_list_progress, this, std::placeholders::_1));
    files_->on_sort().connect(std::bind(&files_page::on_list_sort, this, std::placeholders::_1));

    files_->set_image_list(images_);
}

std::string files_page::on_list_display(const std::pair<int, int> &p)
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

        return to_string(size_str);
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

        return "<unknown priority>";
    }
    default:
        return "<unknown>";
    }
}

void files_page::on_list_item_context_menu(const std::vector<int> &indices)
{
    if (indices.empty())
    {
        return;
    }

    HMENU prioMenu = CreateMenu();
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_MAX, to_wstring(TR("maximum")).c_str());
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_HIGH, to_wstring(TR("high")).c_str());
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_NORMAL, to_wstring(TR("normal")).c_str());
    AppendMenu(prioMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_SKIP, to_wstring(TR("do_not_download")).c_str());

    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_POPUP, (UINT_PTR)prioMenu, to_wstring(TR("priority")).c_str());

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

void files_page::on_list_sort(const std::pair<int, int> &p)
{
    list_view::sort_order_t order = (list_view::sort_order_t)p.second;
    bool asc = order == list_view::sort_order_t::asc;

    std::function<bool(const file_item&, const file_item&)> sort_func;

    switch (p.first)
    {
    case LIST_COLUMN_NAME:
        sort_func = [asc](const file_item &f1, const file_item &f2)
        {
            if (asc) { return f1.name < f2.name; }
            return f1.name > f2.name;
        };
        break;
    case LIST_COLUMN_SIZE:
        sort_func = [asc](const file_item &f1, const file_item &f2)
        {
            if (asc) { return f1.size < f2.size; }
            return f1.size > f2.size;
        };
        break;
    case LIST_COLUMN_PROGRESS:
        sort_func = [asc](const file_item &f1, const file_item &f2)
        {
            if (asc) { return f1.progress < f2.progress; }
            return f1.progress > f2.progress;
        };
        break;
    case LIST_COLUMN_PRIORITY:
        sort_func = [asc](const file_item &f1, const file_item &f2)
        {
            if (asc) { return f1.priority < f2.priority; }
            return f1.priority > f2.priority;
        };
        break;
    }

    if (sort_func)
    {
        std::sort(items_.begin(), items_.end(), sort_func);
    }
}
