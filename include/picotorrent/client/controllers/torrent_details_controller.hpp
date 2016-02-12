#pragma once

#include <memory>

namespace picotorrent
{
namespace core
{
    class peer;
    class torrent;
}
namespace client
{
namespace ui
{
    class main_window;

namespace property_sheets
{
namespace details
{
    class files_page;
    class overview_page;
    class peers_page;
    class trackers_page;
}
}
}
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
        enum active_page_t
        {
            none,
            overview,
            files,
            peers,
            trackers
        };

        void on_files_destroy();
        void on_files_init();
        void on_files_set_prio(const std::pair<int, int>&);
        void on_overview_apply();
        void on_overview_init();

        void on_torrent_updated();

        void set_active_page(active_page_t page);
        void update_files();
        void update_peers();
        void update_trackers();

    private:
        std::shared_ptr<core::torrent> torrent_;
        std::shared_ptr<ui::main_window> wnd_;

        active_page_t active_page_;
        std::unique_ptr<ui::property_sheets::details::files_page> files_;
        std::unique_ptr<ui::property_sheets::details::overview_page> overview_;
        std::unique_ptr<ui::property_sheets::details::peers_page> peers_;
        std::unique_ptr<ui::property_sheets::details::trackers_page> trackers_;
    };
}
}
}
