#include <picotorrent/ui/main_window.hpp>

#include <algorithm>
#include <commctrl.h>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/ui/dialogs/about_dialog.hpp>
#include <picotorrent/ui/notify_icon.hpp>
#include <picotorrent/ui/open_file_dialog.hpp>
#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>
#include <picotorrent/ui/sort.hpp>
#include <picotorrent/ui/task_dialog.hpp>
#include <picotorrent/ui/torrent_list_item.hpp>
#include <picotorrent/ui/torrent_list_view.hpp>
#include <shellapi.h>
#include <strsafe.h>

namespace core = picotorrent::core;
namespace fs = picotorrent::filesystem;
using picotorrent::common::to_wstring;
using picotorrent::ui::dialogs::about_dialog;
using picotorrent::ui::main_window;
using picotorrent::ui::notify_icon;
using picotorrent::ui::open_file_dialog;
using picotorrent::ui::scaler;
using picotorrent::ui::torrent_list_item;
using picotorrent::ui::torrent_list_view;

main_window::main_window()
    : hWnd_(NULL)
{
}

main_window::~main_window()
{
}

void main_window::create()
{
    WNDCLASSEX wnd = { 0 };
    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.cbWndExtra = sizeof(main_window*);
    wnd.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
    wnd.lpfnWndProc = &main_window::wnd_proc_proxy;
    wnd.lpszClassName = TEXT("PicoTorrent/MainWindow");
    wnd.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
    wnd.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClassEx(&wnd);

    hWnd_ = CreateWindowEx(
        0,
        wnd.lpszClassName,
        TEXT("PicoTorrent"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        scaler::x(800),
        scaler::y(200),
        NULL,
        NULL,
        GetModuleHandle(NULL),
        static_cast<LPVOID>(this));
}

void main_window::exit()
{
    DestroyWindow(handle());
}

std::vector<std::shared_ptr<core::torrent>> main_window::get_selected_torrents()
{
    std::vector<core::torrent_ptr> torrents;

    for (int i : list_view_->get_selected_items())
    {
        torrents.push_back(items_.at(i).torrent());
    }

    return torrents;
}

HWND main_window::handle()
{
    return hWnd_;
}

void main_window::on_close(const std::function<bool()> &callback)
{
    close_cb_ = callback;
}

void main_window::on_command(int id, const command_func_t &callback)
{
    commands_.insert({ id, callback });
}

void main_window::on_copydata(const std::function<void(const std::wstring&)> &callback)
{
    copydata_cb_ = callback;
}

void main_window::on_notifyicon_context_menu(const std::function<void(const POINT &p)> &callback)
{
    notifyicon_context_cb_ = callback;
}

void main_window::on_torrent_context_menu(const std::function<void(const POINT &p, const std::vector<std::shared_ptr<core::torrent>>&)> &callback)
{
    torrent_context_cb_ = callback;
}

void main_window::post_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PostMessage(hWnd_, uMsg, wParam, lParam);
}

void main_window::send_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SendMessage(hWnd_, uMsg, wParam, lParam);
}

void main_window::select_all_torrents()
{
    ListView_SetItemState(list_view_->handle(), -1, LVIS_SELECTED, 2);
}

void main_window::hide()
{
    ::ShowWindow(hWnd_, SW_HIDE);
}

LRESULT main_window::wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_TORRENT_ADDED:
    {
        const core::torrent_ptr &t = *(core::torrent_ptr*)lParam;

        torrent_list_item list_item(t);
        items_.push_back(list_item);

        list_view_->set_item_count((int)items_.size());
        break;
    }

    case WM_TORRENT_REMOVED:
    {
        const core::torrent_ptr &t = *(core::torrent_ptr*)lParam;
        auto f = std::find(items_.begin(), items_.end(), torrent_list_item(t));

        if (f != items_.end())
        {
            items_.erase(f);
        }

        list_view_->set_item_count((int)items_.size());
        break;
    }

    case WM_TORRENT_UPDATED:
    {
        const core::torrent_ptr &t = *(core::torrent_ptr*)lParam;

        if (sort_items_)
        {
            sort_items_();
        }

        list_view_->refresh();

        break;
    }

    case WM_TORRENT_FINISHED:
    {
        const core::torrent_ptr &t = *(core::torrent_ptr*)lParam;
        last_finished_save_path_ = to_wstring(t->save_path());
        noticon_->show_balloon(TEXT("Torrent finished"), to_wstring(t->name()));
        break;
    }

    case WM_NOTIFYICON:
    {
        DWORD ev = LOWORD(lParam);
        
        switch (ev)
        {
        case WM_LBUTTONDBLCLK:
        {
            ShowWindow(handle(), SW_RESTORE);
            SetForegroundWindow(handle());

            break;
        }

        case WM_CONTEXTMENU:
        {
            if (notifyicon_context_cb_)
            {
                POINT pt;
                GetCursorPos(&pt);

                notifyicon_context_cb_(pt);
            }
            break;
        }

        case NIN_BALLOONUSERCLICK:
        {
            ShellExecute(handle(), TEXT("open"), last_finished_save_path_.c_str(), NULL, NULL, SW_SHOWNORMAL);
            break;
        }
        }

        break;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        command_map_t::iterator item = commands_.find(id);

        if (item != commands_.end()
            && item->second)
        {
            item->second();
            break;
        }

        switch (LOWORD(wParam))
        {
        case ID_FILE_EXIT:
            exit();
            break;
        case ID_HELP_ABOUT:
        {
            about_dialog dlg;
            dlg.show_modal(hWnd_);
            break;
        }
        }
        break;
    }

    case WM_COPYDATA:
    {
        COPYDATASTRUCT *cds = reinterpret_cast<COPYDATASTRUCT*>(lParam);
        wchar_t *ptr = reinterpret_cast<wchar_t*>(cds->lpData);
        std::wstring args(ptr);

        if (copydata_cb_)
        {
            copydata_cb_(args);
        }

        break;
    }

    case WM_CREATE:
    {
        list_view_ = std::make_unique<torrent_list_view>(hWnd);
        list_view_->create();

        // Add columns
        list_view_->add_column(TEXT("Name"), scaler::x(280), LVCFMT_LEFT);
        list_view_->add_column(TEXT("#"), scaler::x(30), LVCFMT_RIGHT);
        list_view_->add_column(TEXT("Size"), scaler::x(80), LVCFMT_RIGHT);
        list_view_->add_column(TEXT("Status"), scaler::x(120), LVCFMT_LEFT);
        list_view_->add_column(TEXT("Progress"), scaler::x(100), LVCFMT_LEFT);
		list_view_->add_column(TEXT("ETA"), scaler::x(80), LVCFMT_RIGHT);
        list_view_->add_column(TEXT("DL"), scaler::x(80), LVCFMT_RIGHT);
        list_view_->add_column(TEXT("UL"), scaler::x(80), LVCFMT_RIGHT);

        noticon_ = std::make_shared<notify_icon>(hWnd);
        noticon_->add();

        break;
    }

    case WM_CLOSE:
    {
        if (!close_cb_)
        {
            break;
        }

        bool res = close_cb_();
    
        if (!res)
        {
            return FALSE;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    case WM_DESTROY:
    {
        noticon_->remove();

        PostQuitMessage(0);
        break;
    }

    case WM_NOTIFY:
    {
        LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);

        switch (nmhdr->code)
        {
        case LVN_COLUMNCLICK:
        {
            LPNMLISTVIEW lv = reinterpret_cast<LPNMLISTVIEW>(nmhdr);
            int colIndex = lv->iSubItem;
            torrent_list_view::sort_order currentOrder = list_view_->get_column_sort(colIndex);
            torrent_list_view::sort_order newOrder = torrent_list_view::sort_order::asc;

            if (currentOrder == torrent_list_view::sort_order::asc) newOrder = torrent_list_view::sort_order::desc;
            if (currentOrder == torrent_list_view::sort_order::desc) newOrder = torrent_list_view::sort_order::asc;

            for (int i = 0; i < list_view_->get_column_count(); i++)
            {
                list_view_->set_column_sort(i, torrent_list_view::sort_order::none);
            }

            using picotorrent::ui::sort;
            bool isAscending = newOrder == torrent_list_view::sort_order::asc;

            switch (colIndex)
            {
            case COL_NAME:
                sort_items_ = [this, isAscending]()
                {
                    std::sort(items_.begin(), items_.end(), sort::by_name(isAscending));
                };
                break;
            case COL_QUEUE_POSITION:
                sort_items_ = [this, isAscending]()
                {
                    std::sort(items_.begin(), items_.end(), sort::by_queue_position(isAscending));
                };
                break;
            case COL_SIZE:
                sort_items_ = [this, isAscending]()
                {
                    std::sort(items_.begin(), items_.end(), sort::by_size(isAscending));
                };
                break;
            case COL_PROGRESS:
                sort_items_ = [this, isAscending]()
                {
                    std::sort(items_.begin(), items_.end(), sort::by_progress(isAscending));
                };
                break;
			case COL_ETA:
			{
				sort_items_ = [this, isAscending]()
				{
					std::sort(items_.begin(), items_.end(), sort::by_eta(isAscending));
				};
				break;
			}
            case COL_DOWNLOAD_RATE:
                sort_items_ = [this, isAscending]()
                {
                    std::sort(items_.begin(), items_.end(), sort::by_download_rate(isAscending));
                };
                break;
            case COL_UPLOAD_RATE:
                sort_items_ = [this, isAscending]()
                {
                    std::sort(items_.begin(), items_.end(), sort::by_upload_rate(isAscending));
                };
                break;
            }

            if (sort_items_)
            {
                sort_items_();
            }

            list_view_->set_column_sort(colIndex, newOrder);
            list_view_->refresh();

            break;
        }
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* inf = reinterpret_cast<NMLVDISPINFO*>(nmhdr);

            if (inf->item.iItem == -1
                || items_.size() == 0)
            {
                break;
            }

            const torrent_list_item &item = items_.at(inf->item.iItem);
            list_view_->on_getdispinfo(inf, item);

            break;
        }
        case NM_CUSTOMDRAW:
        {
            if (nmhdr->hwndFrom != list_view_->handle()
                || items_.size() == 0)
            {
                break;
            }

            LPNMLVCUSTOMDRAW lpCD = (LPNMLVCUSTOMDRAW)nmhdr;
            const torrent_list_item &item = items_.at(lpCD->nmcd.dwItemSpec);

            return list_view_->on_custom_draw(lpCD, item);
        }
        case NM_RCLICK:
        {
            if (nmhdr->hwndFrom == list_view_->handle())
            {
                if (torrent_context_cb_)
                {
                    POINT pt;
                    GetCursorPos(&pt);

                    torrent_context_cb_(pt, get_selected_torrents());
                }
            }
            break;
        }
        }

        break;
    }

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        // Set new width and height for the torrent list view.
        list_view_->resize(width, height);
        break;
    }

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

LRESULT main_window::wnd_proc_proxy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        LPVOID pThis = pCreateStruct->lpCreateParams;
        SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(pThis));
    }

    main_window* pWnd = reinterpret_cast<main_window*>(GetWindowLongPtr(hWnd, 0));
    return pWnd->wnd_proc(hWnd, uMsg, wParam, lParam);
}
