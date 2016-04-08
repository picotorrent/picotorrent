#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <windows.h>

#include <picotorrent/core/signals/signal.hpp>

namespace picotorrent
{
namespace core
{
    class session;
    class torrent;
}
namespace client
{
namespace ui
{
namespace controls
{
    class list_view;
}

    class notify_icon;
    class taskbar_list;
    class sleep_manager;
    class status_bar;
    class torrent_drop_target;

    class main_window
    {
    public:
        typedef std::function<void()> command_func_t;
        typedef std::map<int, command_func_t> command_map_t;

        main_window(const std::shared_ptr<core::session> &sess);
        ~main_window();

        void torrent_added(const std::shared_ptr<core::torrent>&);
        void torrent_finished(const std::shared_ptr<core::torrent>&);
        void torrent_removed(const std::shared_ptr<core::torrent>&);
        void torrent_updated(const std::shared_ptr<core::torrent>&);

        void exit();
        std::vector<std::shared_ptr<core::torrent>> get_selected_torrents();
        HWND handle();
        void hide();
        void on_close(const std::function<bool()> &callback);
        void on_command(int id, const command_func_t &callback);
        void on_copydata(const std::function<void(const std::wstring&)> &callback);
        core::signals::signal_connector<void, void>& on_destroy();
        void on_notifyicon_context_menu(const std::function<void(const POINT &p)> &callback);
        core::signals::signal_connector<void, void>& on_session_alert_notify();
        void on_torrent_activated(const std::function<void(const std::shared_ptr<core::torrent>&)> &callback);
        void on_torrent_context_menu(const std::function<void(const POINT &p, const std::vector<std::shared_ptr<core::torrent>>&)> &callback);
        core::signals::signal_connector<void, const std::vector<std::string>&>& on_torrents_dropped();
        void post_message(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void send_message(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void select_all_torrents();

    private:
        struct wnd_class_initializer;

        std::string on_list_display(const std::pair<int, int> &p);
        float on_list_progress(const std::pair<int, int> &p);
        void on_list_sort(const std::pair<int, int> &p);

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
        std::vector<std::shared_ptr<core::torrent>> torrents_;
        std::shared_ptr<notify_icon> noticon_;
        std::unique_ptr<controls::list_view> list_view_;
        std::shared_ptr<core::session> sess_;
        std::unique_ptr<status_bar> status_;
        std::shared_ptr<taskbar_list> taskbar_;
        std::string last_finished_save_path_;
        std::unique_ptr<sleep_manager> sleep_manager_;
        std::unique_ptr<torrent_drop_target> drop_target_;

        core::signals::signal<void, void> on_destroy_;
        core::signals::signal<void, void> on_session_alert_notify_;
    };
}
}
}
