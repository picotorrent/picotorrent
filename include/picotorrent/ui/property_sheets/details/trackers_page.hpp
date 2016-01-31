#pragma once

#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>
#include <mutex>
#include <string>
#include <vector>

namespace picotorrent
{
namespace core
{
    class tracker;
}
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

        void refresh(const std::vector<core::tracker> &trackers);

    protected:
        void on_init_dialog();

    private:
        struct tracker_state;

        std::wstring on_list_display(const std::pair<int, int> &p);

        std::mutex update_mtx_;
        std::unique_ptr<controls::list_view> list_;
        std::vector<tracker_state> trackers_;

    };
}
}
}
}
