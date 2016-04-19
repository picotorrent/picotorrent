#pragma once

#include <memory>
#include <string>

namespace picotorrent
{
namespace core
{
    class session;
}
namespace client
{
namespace ui
{
    class main_window;
    class preferences_dialog;

namespace property_sheets
{
namespace preferences
{
    class advanced_page;
    class connection_page;
    class downloads_page;
    class general_page;
    class remote_page;
}
}
}
namespace ws
{
    class websocket_server;
}
namespace controllers
{
    class view_preferences_controller
    {
    public:
        view_preferences_controller(
            const std::shared_ptr<core::session> &sess,
            const std::shared_ptr<ui::main_window> &wnd,
            const std::shared_ptr<ws::websocket_server> &ws);
        ~view_preferences_controller();
        void execute();

    protected:
        void on_advanced_apply();
        void on_advanced_init();

        void on_downloads_apply();
        void on_downloads_init();
        bool on_downloads_validate();

        void on_connection_apply();
        void on_connection_init();
        void on_connection_proxy_type_changed(int type);
        bool on_connection_validate();

        void on_general_apply();
        void on_general_init();

        void on_remote_apply();
        void on_remote_init();

    private:
        void create_run_key();
        void delete_run_key();
        bool has_run_key();
        void restart();
        bool should_restart();

        std::shared_ptr<core::session> sess_;
        std::shared_ptr<ui::main_window> wnd_;
        std::shared_ptr<ws::websocket_server> ws_;
        std::unique_ptr<ui::property_sheets::preferences::advanced_page> adv_page_;
        std::unique_ptr<ui::property_sheets::preferences::connection_page> conn_page_;
        std::unique_ptr<ui::property_sheets::preferences::downloads_page> dl_page_;
        std::unique_ptr<ui::property_sheets::preferences::general_page> gen_page_;
        std::unique_ptr<ui::property_sheets::preferences::remote_page> remote_page_;
    };
}
}
}
