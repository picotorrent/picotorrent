#include <picotorrent/client/ui/main_window.hpp>

#include <algorithm>
#include <commctrl.h>
#include <picotorrent/core/string_operations.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/session_metrics.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/core/filesystem/path.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/controls/list_view.hpp>
#include <picotorrent/client/ui/dialogs/about_dialog.hpp>
#include <picotorrent/client/ui/notify_icon.hpp>
#include <picotorrent/client/ui/open_file_dialog.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/scaler.hpp>
#include <picotorrent/client/ui/sleep_manager.hpp>
#include <picotorrent/client/ui/status_bar.hpp>
#include <picotorrent/client/ui/task_dialog.hpp>
#include <picotorrent/client/ui/taskbar_list.hpp>
#include <picotorrent/client/ui/torrent_drop_target.hpp>
#include <chrono>
#include <shellapi.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <strsafe.h>

#define PT_SESSION_NOTIFY WM_USER+1337

#define COLUMN_NAME 1
#define COLUMN_QUEUE_POSITION 2
#define COLUMN_SIZE 3
#define COLUMN_STATUS 4
#define COLUMN_PROGRESS 5
#define COLUMN_ETA 6
#define COLUMN_DL 7
#define COLUMN_UL 8

namespace core = picotorrent::core;
namespace fs = picotorrent::core::filesystem;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;
using picotorrent::core::to_wstring;
using picotorrent::client::ui::controls::list_view;
using picotorrent::client::ui::dialogs::about_dialog;
using picotorrent::client::ui::main_window;
using picotorrent::client::ui::notify_icon;
using picotorrent::client::ui::open_file_dialog;
using picotorrent::client::ui::scaler;
using picotorrent::client::ui::taskbar_list;
using picotorrent::client::ui::sleep_manager;
using picotorrent::client::ui::torrent_drop_target;

const UINT main_window::TaskbarButtonCreated = RegisterWindowMessage(L"TaskbarButtonCreated");

struct main_window::wnd_class_initializer
{
    wnd_class_initializer()
    {
        WNDCLASSEX wnd = { 0 };
        wnd.cbSize = sizeof(WNDCLASSEX);
        wnd.cbWndExtra = sizeof(main_window*);
        wnd.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
        wnd.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
        wnd.lpfnWndProc = &main_window::wnd_proc_proxy;
        wnd.lpszClassName = TEXT("PicoTorrent/MainWindow");
        wnd.style = CS_HREDRAW | CS_VREDRAW;

        RegisterClassEx(&wnd);
    }
};

main_window::main_window(const std::shared_ptr<core::session> &sess)
    : hWnd_(NULL),
    sess_(sess),
    sleep_manager_(std::make_unique<sleep_manager>())
{
    static wnd_class_initializer instance;

    hWnd_ = CreateWindowEx(
        0,
        L"PicoTorrent/MainWindow",
        TEXT("PicoTorrent"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        scaler::x(800),
        scaler::y(200),
        NULL,
        NULL,
        GetModuleHandle(NULL),
        static_cast<LPVOID>(this));

    // Create the drop target
    drop_target_ = std::make_unique<torrent_drop_target>(hWnd_);

    // Create main menu
    HMENU file = CreateMenu();
    AppendMenu(file, MF_STRING, ID_FILE_ADD_TORRENT, TR("amp_add_torrent"));
    AppendMenu(file, MF_STRING, ID_FILE_ADD_MAGNET_LINK, TR("amp_add_magnet_link_s"));
    AppendMenu(file, MF_SEPARATOR, 0, NULL);
    AppendMenu(file, MF_STRING, ID_FILE_EXIT, TR("amp_exit"));

    HMENU view = CreateMenu();
    AppendMenu(view, MF_STRING, ID_VIEW_PREFERENCES, TR("amp_preferences"));

    HMENU help = CreateMenu();
    AppendMenu(help, MF_STRING, ID_HELP_CHECK_FOR_UPDATE, TR("amp_check_for_update"));
    AppendMenu(help, MF_SEPARATOR, 0, NULL);
    AppendMenu(help, MF_STRING, ID_HELP_ABOUT, TR("amp_about"));

    HMENU menuBar = CreateMenu();
    AppendMenu(menuBar, MF_POPUP, (UINT_PTR)file, TR("amp_file"));
    AppendMenu(menuBar, MF_POPUP, (UINT_PTR)view, TR("amp_view"));
    AppendMenu(menuBar, MF_POPUP, (UINT_PTR)help, TR("amp_help"));

    SetMenu(hWnd_, menuBar);
}

main_window::~main_window()
{
}

void main_window::exit()
{
    DestroyWindow(handle());
}

void main_window::torrent_added(const std::shared_ptr<core::torrent> &t)
{
    torrents_.push_back(t);
    if (sort_items_) { sort_items_(); }
    list_view_->set_item_count((int)torrents_.size());
    status_->set_torrent_count((int)torrents_.size());
}

void main_window::torrent_finished(const std::shared_ptr<core::torrent> &t)
{
    last_finished_save_path_ = to_wstring(t->save_path());
    noticon_->show_balloon(TR("torrent_finished"), to_wstring(t->name()));
}

void main_window::torrent_removed(const std::shared_ptr<core::torrent> &t)
{
    auto &f = std::find(torrents_.begin(), torrents_.end(), t);

    if (f != torrents_.end())
    {
        torrents_.erase(f);
        list_view_->set_item_count((int)torrents_.size());
        status_->set_torrent_count((int)torrents_.size());
    }
}

void main_window::torrent_updated(const std::shared_ptr<core::torrent> &t)
{
    if (sort_items_) { sort_items_(); }
    list_view_->refresh();
}

std::vector<std::shared_ptr<core::torrent>> main_window::get_selected_torrents()
{
    std::vector<core::torrent_ptr> torrents;

    for (int i : list_view_->get_selection())
    {
        torrents.push_back(torrents_.at(i));
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

signal_connector<void, void>& main_window::on_destroy()
{
    return on_destroy_;
}

void main_window::on_notifyicon_context_menu(const std::function<void(const POINT &p)> &callback)
{
    notifyicon_context_cb_ = callback;
}

signal_connector<void, void>& main_window::on_session_alert_notify()
{
    return on_session_alert_notify_;
}

void main_window::on_torrent_activated(const std::function<void(const std::shared_ptr<core::torrent>&)> &callback)
{
    torrent_activated_cb = callback;
}


void main_window::on_torrent_context_menu(const std::function<void(const POINT &p, const std::vector<std::shared_ptr<core::torrent>>&)> &callback)
{
    torrent_context_cb_ = callback;
}

signal_connector<void, const std::vector<core::filesystem::path>&>& main_window::on_torrents_dropped()
{
    return drop_target_->on_torrents_dropped();
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
    if (uMsg == TaskbarButtonCreated)
    {
        taskbar_ = std::make_shared<taskbar_list>(handle());
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
    case PT_SESSION_NOTIFY:
    {
        on_session_alert_notify_.emit();
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
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);

        // Add status bar
        status_ = std::make_unique<status_bar>(hWnd);
        ShowWindow(status_->handle(), SW_SHOWNORMAL);
        UpdateWindow(status_->handle());

        RECT rcStatus;
        GetClientRect(status_->handle(), &rcStatus);
        int statusHeight = rcStatus.bottom - rcStatus.top;

        HWND hList = CreateWindowEx(
            0,
            WC_LISTVIEW,
            0,
            WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT,
            0,
            0,
            rcClient.right - rcClient.left,
            (rcClient.bottom - rcClient.top) - statusHeight,
            hWnd,
            NULL,
            GetModuleHandle(NULL),
            NULL);

        list_view_ = std::make_unique<list_view>(hList);
        list_view_->on_display().connect(std::bind(&main_window::on_list_display, this, std::placeholders::_1));
        list_view_->on_progress().connect(std::bind(&main_window::on_list_progress, this, std::placeholders::_1));
        list_view_->on_sort().connect(std::bind(&main_window::on_list_sort, this, std::placeholders::_1));

        // Add columns
        list_view_->add_column(COLUMN_NAME,           TR("name"),           scaler::x(280), list_view::text);
        list_view_->add_column(COLUMN_QUEUE_POSITION, TR("queue_position"), scaler::x(30),  list_view::number);
        list_view_->add_column(COLUMN_SIZE,           TR("size"),           scaler::x(80),  list_view::number);
        list_view_->add_column(COLUMN_STATUS,         TR("status"),         scaler::x(120), list_view::text);
        list_view_->add_column(COLUMN_PROGRESS,       TR("progress"),       scaler::x(100), list_view::progress);
        list_view_->add_column(COLUMN_ETA,            TR("eta"),            scaler::x(80),  list_view::number);
        list_view_->add_column(COLUMN_DL,             TR("dl"),             scaler::x(80),  list_view::number);
        list_view_->add_column(COLUMN_UL,             TR("ul"),             scaler::x(80),  list_view::number);

        noticon_ = std::make_shared<notify_icon>(hWnd);
        noticon_->add();

        // Set update timer
        SetTimer(hWnd, 6060, 1000, NULL);

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
        on_destroy_.emit();
        noticon_->remove();

        PostQuitMessage(0);
        break;
    }

    case WM_NOTIFY:
    {
        LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);

        switch (nmhdr->code)
        {
        case LVN_ITEMACTIVATE:
        {
            if (torrent_activated_cb)
            {
                int index = list_view_->get_selection()[0];
                const std::shared_ptr<core::torrent> &t = torrents_.at(index);
                torrent_activated_cb(t);
            }
            break;
        }
        case NM_RCLICK:
        {
            if (nmhdr->hwndFrom == list_view_->handle())
            {
                std::vector<int> selectedItems = list_view_->get_selection();

                if (selectedItems.size() == 0)
                {
                    break;
                }

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
        SendMessage(status_->handle(), WM_SIZE, NULL, NULL);

        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        RECT rcStatus;
        GetClientRect(status_->handle(), &rcStatus);
        int statusHeight = rcStatus.bottom - rcStatus.top;

        // Set new width and height for the torrent list view.
        list_view_->resize(width, height - statusHeight);
        break;
    }

    case WM_TIMER:
    {
        // Post session updates
        sess_->post_updates();

        uint64_t active_done = 0;
        uint64_t active_wanted = 0;
        uint64_t paused_done = 0;
        uint64_t paused_wanted = 0;
        int dl_rate = 0;
        int ul_rate = 0;
        bool has_error = false;

        for (const core::torrent_ptr &t : torrents_)
        {
            dl_rate += t->download_rate();
            ul_rate += t->upload_rate();

            if (t->has_error())
            {
                has_error = true;
            }

            if (!t->is_seeding() && !t->is_paused())
            {
                active_done += t->total_wanted_done();
                active_wanted += t->total_wanted();
            }
            else if (!t->is_seeding() && t->is_paused())
            {
                paused_done += t->total_wanted_done();
                paused_wanted += t->total_wanted();
            }
        }

        std::shared_ptr<core::session_metrics> metrics = sess_->metrics();
        status_->set_transfer_rates(dl_rate, ul_rate);

        sleep_manager_->refresh(active_wanted + active_done > 0 ? true : false);

        // If we start PicoTorrent minimized, the taskbar may not have been created
        // yet, so check for null here.
        if (taskbar_ != nullptr)
        {
            if (active_wanted - active_done > 0)
            {
                taskbar_->set_progress_state(has_error ? TBPF_ERROR : TBPF_NORMAL);
                taskbar_->set_progress_value(active_done, active_wanted);
            }
            else if (paused_wanted - paused_done > 0)
            {
                taskbar_->set_progress_state(has_error ? TBPF_ERROR : TBPF_PAUSED);
                taskbar_->set_progress_value(paused_done, paused_wanted);
            }
            else
            {
                taskbar_->set_progress_state(TBPF_NOPROGRESS);
            }
        }
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

std::wstring main_window::on_list_display(const std::pair<int, int> &p)
{
    const core::torrent_ptr &t = torrents_[p.second];

    switch (p.first)
    {
    case COLUMN_NAME:
        return to_wstring(t->name());
    case COLUMN_QUEUE_POSITION:
        if (t->queue_position() < 0)
        {
            return L"-";
        }

        return std::to_wstring(t->queue_position() + 1);
    case COLUMN_SIZE:
        TCHAR size[128];
        StrFormatByteSize64(t->size(), size, ARRAYSIZE(size));
        return size;
    case COLUMN_STATUS:
    {
        switch (t->state())
        {
        case core::torrent_state::state_t::checking_resume_data:
            return TR("state_checking_resume_data");
        case core::torrent_state::state_t::downloading:
            return TR("state_downloading");
        case core::torrent_state::state_t::downloading_checking:
            return TR("state_downloading_checking");
        case core::torrent_state::state_t::downloading_forced:
            return TR("state_downloading_forced");
        case core::torrent_state::state_t::downloading_metadata:
            return TR("state_downloading_metadata");
        case core::torrent_state::state_t::downloading_paused:
            return TR("state_downloading_paused");
        case core::torrent_state::state_t::downloading_queued:
            return TR("state_downloading_queued");
        case core::torrent_state::state_t::downloading_stalled:
            return TR("state_downloading_stalled");
        case core::torrent_state::state_t::error:
            TCHAR err[1024];
            StringCchPrintf(
                err,
                ARRAYSIZE(err),
                TR("state_error"),
                to_wstring(t->error_message()).c_str());
            return err;
        case core::torrent_state::state_t::unknown:
            return TR("state_unknown");
        case core::torrent_state::state_t::uploading:
            return TR("state_uploading");
        case core::torrent_state::state_t::uploading_checking:
            return TR("state_uploading_checking");
        case core::torrent_state::state_t::uploading_forced:
            return TR("state_uploading_forced");
        case core::torrent_state::state_t::uploading_paused:
            return TR("state_uploading_paused");
        case core::torrent_state::state_t::uploading_queued:
            return TR("state_uploading_queued");
        case core::torrent_state::state_t::uploading_stalled:
            return TR("state_uploading_stalled");
        }

        return L"<unknown state>";
    }
    case COLUMN_ETA:
    {
        std::chrono::seconds next(t->eta());

        if (next.count() < 0)
        {
            return L"-";
        }

        std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(next);
        std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(next - hours_left);
        std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(next - hours_left - min_left);

        TCHAR t[100];
        StringCchPrintf(
            t,
            ARRAYSIZE(t),
            L"%dh %dm %ds",
            hours_left.count(),
            min_left.count(),
            sec_left.count());
        return t;
    }
    case COLUMN_DL:
    case COLUMN_UL:
    {
        int rate = p.first == COLUMN_DL ? t->download_rate() : t->upload_rate();

        if (rate < 1024)
        {
            return L"-";
        }

        TCHAR speed[128];
        StrFormatByteSize64(rate, speed, ARRAYSIZE(speed));
        StringCchPrintf(speed, ARRAYSIZE(speed), L"%s/s", speed);
        return speed;
    }
    }

    return L"<unknown>";
}

float main_window::on_list_progress(const std::pair<int, int> &p)
{
    const core::torrent_ptr &t = torrents_[p.second];

    switch (p.first)
    {
    case COLUMN_PROGRESS:
        return t->progress();
    }

    return 0;
}

void main_window::on_list_sort(const std::pair<int, int> &p)
{
    list_view::sort_order_t order = (list_view::sort_order_t)p.second;
    bool asc = order == list_view::sort_order_t::asc;

    std::function<bool(const core::torrent_ptr&, const core::torrent_ptr&)> sort_func;

    switch (p.first)
    {
    case COLUMN_NAME:
    {
        sort_func = [asc](const core::torrent_ptr &t1, const core::torrent_ptr &t2)
        {
            if (asc) { return t1->name() < t2->name(); }
            return t1->name() > t2->name();
        };
        break;
    }
    case COLUMN_QUEUE_POSITION:
    {
        sort_func = [asc](const core::torrent_ptr &t1, const core::torrent_ptr &t2)
        {
            if (asc) { return t1->queue_position() < t2->queue_position(); }
            return t1->queue_position() > t2->queue_position();
        };
        break;
    }
    case COLUMN_SIZE:
    {
        sort_func = [asc](const core::torrent_ptr &t1, const core::torrent_ptr &t2)
        {
            if (asc) { return t1->size() < t2->size(); }
            return t1->size() > t2->size();
        };
        break;
    }
    case COLUMN_STATUS:
    {
        sort_func = [asc](const core::torrent_ptr &t1, const core::torrent_ptr &t2)
        {
            if (asc) { return t1->state() < t2->state(); }
            return t1->state() > t2->state();
        };
        break;
    }
    case COLUMN_PROGRESS:
    {
        sort_func = [asc](const core::torrent_ptr &t1, const core::torrent_ptr &t2)
        {
            if (asc) { return t1->progress() < t2->progress(); }
            return t1->progress() > t2->progress();
        };
        break;
    }
    case COLUMN_ETA:
    {
        sort_func = [asc](const core::torrent_ptr &t1, const core::torrent_ptr &t2)
        {
            if (asc) { return t1->eta() < t2->eta(); }
            return t1->eta() > t2->eta();
        };
        break;
    }
    case COLUMN_DL:
    {
        sort_func = [asc](const core::torrent_ptr &t1, const core::torrent_ptr &t2)
        {
            if (asc) { return t1->download_rate() < t2->download_rate(); }
            return t1->download_rate() > t2->download_rate();
        };
        break;
    }
    case COLUMN_UL:
    {
        sort_func = [asc](const core::torrent_ptr &t1, const core::torrent_ptr &t2)
        {
            if (asc) { return t1->upload_rate() < t2->upload_rate(); }
            return t1->upload_rate() > t2->upload_rate();
        };
        break;
    }
    }

    if (sort_func)
    {
        sort_items_ = [this, sort_func]()
        {
            std::sort(torrents_.begin(), torrents_.end(), sort_func);
        };
        sort_items_();
    }
}
