#pragma once

#include <memory>
#include <string>

namespace picotorrent
{
namespace ui
{
    class main_window;
    class preferences_dialog;

namespace property_sheets
{
namespace preferences
{
    class connection_page;
    class downloads_page;
}
}
}
namespace app
{
namespace controllers
{
    class view_preferences_controller
    {
    public:
        view_preferences_controller(const std::shared_ptr<ui::main_window> &wnd);
        ~view_preferences_controller();
        void execute();

    protected:
        void apply_downloads_page();
        void init_connection_page();
        void init_downloads_page();

        void on_connection_proxy_type_changed(int type);

    private:
        std::shared_ptr<ui::main_window> wnd_;
        std::unique_ptr<ui::property_sheets::preferences::connection_page> conn_page_;
        std::unique_ptr<ui::property_sheets::preferences::downloads_page> dl_page_;
    };
}
}
}
