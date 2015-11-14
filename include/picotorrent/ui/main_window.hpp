#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <windows.h>

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
    class torrent_list_item;
    class torrent_list_view;

    class main_window
    {
    public:
        typedef std::function<void()> command_func_t;
        typedef std::map<int, command_func_t> command_map_t;

        main_window();
        ~main_window();

        void create();
        void exit();
        HWND handle();
        void hide();
        void on_command(int id, const command_func_t &callback);
        void on_copydata(const std::function<void(const std::wstring&)> &callback);
        void on_notifyicon_context_menu(const std::function<void(const POINT &p)> &callback);
        void on_torrent_context_menu(const std::function<void(const POINT &p, const std::shared_ptr<core::torrent>&)> &callback);
        void post_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        LRESULT CALLBACK wnd_proc(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
        static LRESULT CALLBACK wnd_proc_proxy(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);

        HWND hWnd_;
        command_map_t commands_;
        std::function<void(const std::wstring&)> copydata_cb_;
        std::function<void(const POINT &p)> notifyicon_context_cb_;
        std::function<void(const POINT &p, const std::shared_ptr<core::torrent>&)> torrent_context_cb_;
        std::function<void()> sort_items_;
        std::vector<torrent_list_item> items_;
        std::shared_ptr<notify_icon> noticon_;
        std::unique_ptr<torrent_list_view> list_view_;
    };
}
}
