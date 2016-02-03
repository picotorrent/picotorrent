#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <windows.h>

#include <picotorrent/common/signals/signal.hpp>

#define WM_TORRENT_ADDED WM_USER+1
#define WM_TORRENT_REMOVED WM_USER+2
#define WM_TORRENT_UPDATED WM_USER+3
#define WM_TORRENT_FINISHED WM_USER+4

namespace picotorrent
{
namespace core
{
    class torrent;
}
namespace ui
{
    class notify_icon;
    class taskbar_list;
    class torrent_list_item;
    class torrent_list_view;
    class sleep_manager;

    class main_window
    {
    public:
        typedef std::function<void()> command_func_t;
        typedef std::map<int, command_func_t> command_map_t;

        main_window();
        ~main_window();

        void torrent_added(const std::shared_ptr<core::torrent>&);
        void torrent_finished(const std::shared_ptr<core::torrent>&);
        void torrent_removed(const std::shared_ptr<core::torrent>&);
        void torrent_updated(const std::shared_ptr<core::torrent>&);

        void create();
        void exit();
        std::vector<std::shared_ptr<core::torrent>> get_selected_torrents();
        HWND handle();
        void hide();
        void on_close(const std::function<bool()> &callback);
        void on_command(int id, const command_func_t &callback);
        void on_copydata(const std::function<void(const std::wstring&)> &callback);
        void on_notifyicon_context_menu(const std::function<void(const POINT &p)> &callback);
        common::signals::signal_connector<void, void>& on_session_alert_notify();
        void on_torrent_activated(const std::function<void(const std::shared_ptr<core::torrent>&)> &callback);
        void on_torrent_context_menu(const std::function<void(const POINT &p, const std::vector<std::shared_ptr<core::torrent>>&)> &callback);
        void post_message(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void send_message(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void select_all_torrents();

    private:
        LRESULT CALLBACK wnd_proc(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
        static LRESULT CALLBACK wnd_proc_proxy(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);

        static const UINT TaskbarButtonCreated;

        HWND hWnd_;
        command_map_t commands_;
        std::function<bool()> close_cb_;
        std::function<void(const std::wstring&)> copydata_cb_;
        std::function<void(const POINT &p)> notifyicon_context_cb_;
        std::function<void(const std::shared_ptr<core::torrent>&)> torrent_activated_cb;
        std::function<void(const POINT &p, const std::vector<std::shared_ptr<core::torrent>>&)> torrent_context_cb_;
        std::function<void()> sort_items_;
        std::vector<torrent_list_item> items_;
        std::shared_ptr<notify_icon> noticon_;
        std::unique_ptr<torrent_list_view> list_view_;
        std::shared_ptr<taskbar_list> taskbar_;
        std::wstring last_finished_save_path_;
        std::unique_ptr<sleep_manager> sleep_manager_;

        common::signals::signal<void, void> on_session_alert_notify_;
    };
}
}
