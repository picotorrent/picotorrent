#pragma once

#include <memory>

namespace picotorrent
{
namespace core
{
    class torrent;
}
namespace ui
{
    class main_window;

namespace property_sheets
{
namespace details
{
    class files_page;
    class overview_page;
}
}
}
namespace app
{
namespace controllers
{
    class torrent_details_controller
    {
    public:
        torrent_details_controller(const std::shared_ptr<ui::main_window> &wnd,
            const std::shared_ptr<core::torrent> &torrent);
        ~torrent_details_controller();

        void execute();

    protected:
        void on_files_init();
        void on_overview_apply();
        void on_overview_init();

    private:
        std::shared_ptr<core::torrent> torrent_;
        std::shared_ptr<ui::main_window> wnd_;

        std::unique_ptr<ui::property_sheets::details::files_page> files_;
        std::unique_ptr<ui::property_sheets::details::overview_page> overview_;
    };
}
}
}
