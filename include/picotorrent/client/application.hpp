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
    class executor;

    class application : public common::application
    {
    public:
        DLL_EXPORT application();
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
        void activate_other_instance(const common::command_line &cmd);
        bool is_service_running();
        void on_unhandled_exception(const std::string& stacktrace);

        std::shared_ptr<executor> executor_;
    };
}
}
