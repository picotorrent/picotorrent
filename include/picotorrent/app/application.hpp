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

namespace ui
{
    class main_window;
}

namespace app
{
    namespace controllers
    {
        class application_update_controller;
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

    private:
        bool on_close();
        void on_command_line_args(const std::wstring &args);
        void on_file_add_torrent();
        void on_notifyicon_context_menu(const POINT &p);
        void on_remove_torrents_accelerator(bool remove_data);
        void on_select_all_accelerator();
        void on_torrent_activated(const std::shared_ptr<core::torrent> &torrent);
        void on_torrent_context_menu(const POINT &p, const std::vector<std::shared_ptr<core::torrent>> &torrents);
        void on_view_preferences();

        void on_unhandled_exception(const std::string& stacktrace);
        void torrent_added(const std::shared_ptr<core::torrent> &torrent);
        void torrent_finished(const std::shared_ptr<core::torrent> &torrent);
        void torrent_removed(const std::shared_ptr<core::torrent> &torrent);
        void torrent_updated(const std::shared_ptr<core::torrent> &torrent);

        HANDLE mtx_;
        HACCEL accelerators_;
        std::shared_ptr<ui::main_window> main_window_;
        std::shared_ptr<core::session> sess_;

        std::shared_ptr<controllers::application_update_controller> updater_;
    };
}
}
