#pragma once

#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>
#include <picotorrent/core/signals/signal.hpp>
#include <string>
#include <vector>

namespace picotorrent
{
namespace core
{
    class torrent;
    class tracker;
}
namespace client
{
namespace ui
{
namespace controls
{
    class list_view;
}
namespace property_sheets
{
namespace details
{
    class trackers_page : public property_sheet_page
    {
    public:
        trackers_page();
        ~trackers_page();

        void refresh(const std::shared_ptr<core::torrent> &torrent);

        inline core::signals::signal_connector<void, void>& on_add_tracker() { return on_add_tracker_; }
        inline core::signals::signal_connector<void, const std::vector<std::string>&>& on_remove_trackers() {return on_remove_trackers_;}
    protected:
        void on_init_dialog();

    private:
        std::string on_list_display(const std::pair<int, int> &p);
        void on_trackers_context_menu(const std::vector<int> &items);

        struct tracker_state;

        core::signals::signal<void, void> on_add_tracker_;
        core::signals::signal<void, const std::vector<std::string>&> on_remove_trackers_;
        std::unique_ptr<controls::list_view> list_;
        std::vector<tracker_state> trackers_;

    };
}
}
}
}
}
