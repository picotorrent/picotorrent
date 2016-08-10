#pragma once

#include <windows.h>

#include <memory>
#include <vector>
#include <picotorrent/client/executor.hpp>

namespace picotorrent
{
namespace common
{
namespace ws
{
    class websocket_server;
}
    class command_line;
}
namespace core
{
    class session;
    class torrent;
}
namespace extensibility
{
    class plugin_engine;
}
namespace client
{
namespace controllers
{
    class application_update_controller;
}
namespace ui
{
    class main_window;
}

    class normal_executor : public executor
    {
    public:
        normal_executor(
            const std::shared_ptr<core::session> &session,
            const std::shared_ptr<common::ws::websocket_server> &ws_server);

        int run(const common::command_line &cmd);

        void notification_available();
        void torrent_added(const std::shared_ptr<core::torrent>&);
        void torrent_finished(const std::shared_ptr<core::torrent>&);
        void torrent_removed(const std::shared_ptr<core::torrent>&);
        void torrent_updated(const std::vector<std::shared_ptr<core::torrent>>&);

    private:
        bool on_close();
        void on_command_line_args(const common::command_line &cmd);
        void on_copydata(const std::wstring &args);
        void on_destroy();
        void on_file_add_magnet_link();
        void on_file_add_torrent();
        void on_notifyicon_context_menu(const POINT &p);
        void on_remove_torrents_accelerator(bool remove_data);
        void on_select_all_accelerator();
        void on_session_alert_notify();
        void on_torrent_activated(const std::shared_ptr<core::torrent> &torrent);
        void on_torrent_context_menu(const POINT &p, const std::vector<std::shared_ptr<core::torrent>> &torrents);
        void on_torrents_dropped(const std::vector<std::string> &files);
        void on_view_preferences();

        HACCEL accelerators_;

        std::shared_ptr<ui::main_window> main_window_;
        std::shared_ptr<core::session> session_;
        std::shared_ptr<common::ws::websocket_server> ws_server_;
        std::shared_ptr<extensibility::plugin_engine> plugin_engine_;
    };
}
}
