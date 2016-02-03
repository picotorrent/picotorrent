#include <picotorrent/ui/property_sheets/details/trackers_page.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/tracker.hpp>
#include <picotorrent/ui/controls/list_view.hpp>
#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <cassert>
#include <shlwapi.h>
#include <strsafe.h>

#define LIST_COLUMN_URL    1
#define LIST_COLUMN_STATUS 2
#define LIST_COLUMN_PEERS  3

using picotorrent::common::to_wstring;
using picotorrent::core::tracker;
using picotorrent::ui::controls::list_view;
using picotorrent::ui::property_sheets::details::trackers_page;
using picotorrent::ui::scaler;

struct trackers_page::tracker_state
{
    tracker_state(const tracker &t)
        : tracker(t)
    {
    }

    tracker tracker;
    bool dirty;
};

trackers_page::trackers_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_TRACKERS);
    set_title_id(IDS_DETAILS_TRACKERS_TITLE);
}

trackers_page::~trackers_page()
{
}

void trackers_page::refresh(const std::vector<tracker> &trackers)
{
    for (tracker_state &ts : trackers_)
    {
        ts.dirty = false;
    }

    for (const tracker &t : trackers)
    {
        // TODO: std::find_if uses moderate CPU here, and in a loop as well. maybe a std::map is better for peers_.
        auto &item = std::find_if(trackers_.begin(), trackers_.end(), [t](const tracker_state &ts) { return t.url() == ts.tracker.url(); });

        if (item != trackers_.end())
        {
            item->tracker = t;
            item->dirty = true;
        }
        else
        {
            tracker_state state(t);
            state.dirty = true;
            trackers_.push_back(state);
        }
    }

    trackers_.erase(std::remove_if(trackers_.begin(), trackers_.end(),
        [](const tracker_state &t)
    {
        return !t.dirty;
    }), trackers_.end());

    assert(trackers_.size() == trackers.size());
    list_->set_item_count((int)trackers_.size());
}

void trackers_page::on_init_dialog()
{
    HWND hList = GetDlgItem(handle(), ID_DETAILS_TRACKERS_LIST);
    list_ = std::make_unique<list_view>(hList);

    list_->add_column(LIST_COLUMN_URL,    L"Url",    scaler::x(240));
    list_->add_column(LIST_COLUMN_STATUS, L"Status", scaler::x(120));
    //list_->add_column(LIST_COLUMN_PEERS,  L"Peers",  scaler::x(80), list_view::number);

    list_->on_display().connect(std::bind(&trackers_page::on_list_display, this, std::placeholders::_1));
}

std::wstring trackers_page::on_list_display(const std::pair<int, int> &p)
{
    const tracker_state &t = trackers_[p.second];

    switch (p.first)
    {
    case LIST_COLUMN_URL:
        return to_wstring(t.tracker.url());
    case LIST_COLUMN_STATUS:
        switch (t.tracker.status())
        {
        case tracker::not_working:
            return L"Not working";
        case tracker::updating:
            return L"Updating";
        case tracker::working:
            return L"Working";
        case tracker::unknown:
        default:
            return L"Unknown";
        }
        break;
    default:
        return L"<unknown column>";
    }
}
