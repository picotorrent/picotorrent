#pragma once

#include <memory>
#include <string>
#include <vector>
#include <windows.h>

namespace picotorrent
{
namespace core
{
    class session;
    class torrent;
}

namespace client
{
namespace controllers
{
    class application_update_controller;
}

namespace plugins
{
    class plugin_host;
}

namespace ui
{
    class main_window;
}

    class application
    {
    public:
        application();
        ~application();

        void activate_other_instance(const std::wstring &args);
        bool init();
        bool is_single_instance();
        int run(const std::wstring &args);
        static void wait_for_restart(const std::wstring &args);

    private:
        void on_check_for_update();
        bool on_close();
        void on_command_line_args(const std::wstring &args);
        void on_destroy();
        void on_file_add_torrent();
        void on_notifyicon_context_menu(const POINT &p);
        void on_remove_torrents_accelerator(bool remove_data);
        void on_select_all_accelerator();
        void on_session_alert_notify();
        void on_torrent_activated(const std::shared_ptr<core::torrent> &torrent);
        void on_torrent_context_menu(const POINT &p, const std::vector<std::shared_ptr<core::torrent>> &torrents);
        void on_view_preferences();

        void on_unhandled_exception(const std::string& stacktrace);

        HANDLE mtx_;
        HACCEL accelerators_;
        std::shared_ptr<ui::main_window> main_window_;
        std::shared_ptr<core::session> sess_;
        std::shared_ptr<plugins::plugin_host> plugin_host_;

        std::shared_ptr<controllers::application_update_controller> updater_;
    };
}
}
