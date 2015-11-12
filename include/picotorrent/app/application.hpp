#pragma once

#include <memory>
#include <string>
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
        void on_command_line_args(const std::wstring &args);
        void on_file_add_torrent();
        void on_torrent_context_menu(const POINT &p, const std::shared_ptr<core::torrent> &torrent);
        void on_view_preferences();

        void on_unhandled_exception(const std::string& stacktrace);
        void torrent_added(const std::shared_ptr<core::torrent> &torrent);
        void torrent_finished(const std::shared_ptr<core::torrent> &torrent);
        void torrent_removed(const std::shared_ptr<core::torrent> &torrent);
        void torrent_updated(const std::shared_ptr<core::torrent> &torrent);

        HANDLE mtx_;
        std::shared_ptr<ui::main_window> main_window_;
        std::shared_ptr<core::session> sess_;
    };
}
}
