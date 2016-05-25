#pragma once

#include <memory>
#include <string>
#include <vector>
#include <windows.h>

#include <picotorrent/common/application.hpp>

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
namespace ui
{
    class main_window;
}

    class application : public common::application
    {
    public:
        DLL_EXPORT application();
        DLL_EXPORT ~application();

        DLL_EXPORT void activate_other_instance(const std::wstring &args);
        DLL_EXPORT bool is_single_instance();
        DLL_EXPORT bool is_service_running();
        DLL_EXPORT int run_degraded();
        DLL_EXPORT static void wait_for_restart(const common::command_line &cmd);

    protected:
        bool pre_init();
        
        bool on_init();
        void on_notifications_available();
        int on_run(const common::command_line &cmd);
        void on_torrent_added(const std::shared_ptr<core::torrent> &torrent);
        void on_torrent_finished(const std::shared_ptr<core::torrent> &torrent);
        void on_torrent_removed(const std::shared_ptr<core::torrent> &torrent);
        void on_torrent_updated(const std::vector<std::shared_ptr<core::torrent>> &torrents);

    private:
        void on_check_for_update();
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

        void on_unhandled_exception(const std::string& stacktrace);

        HANDLE mtx_;
        HACCEL accelerators_;

        std::shared_ptr<ui::main_window> main_window_;
        std::shared_ptr<controllers::application_update_controller> updater_;
    };
}
}
