#include <picotorrent/client/ui/property_sheets/details/trackers_page.hpp>

#include <picotorrent/core/string_operations.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/core/tracker.hpp>
#include <picotorrent/core/tracker_status.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/controls/list_view.hpp>
#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/scaler.hpp>

#include <cassert>
#include <chrono>
#include <shlwapi.h>
#include <strsafe.h>

#define LIST_COLUMN_URL    1
#define LIST_COLUMN_STATUS 2
#define LIST_COLUMN_UPDATE 3
#define LIST_COLUMN_PEERS  4
#define LIST_COLUMN_SCRAPE 5

using picotorrent::core::to_wstring;
using picotorrent::core::torrent;
using picotorrent::core::tracker;
using picotorrent::core::tracker_status;
using picotorrent::client::ui::controls::list_view;
using picotorrent::client::ui::property_sheets::details::trackers_page;
using picotorrent::client::ui::scaler;

struct trackers_page::tracker_state
{
    tracker_state(const tracker &t)
        : tracker(t)
    {
    }

    tracker tracker;
    tracker_status status;
    bool dirty;
};

trackers_page::trackers_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_TRACKERS);
    set_title(TR("trackers"));
}

trackers_page::~trackers_page()
{
}

void trackers_page::refresh(const std::shared_ptr<torrent> &torrent)
{
    for (tracker_state &ts : trackers_)
    {
        ts.dirty = false;
    }

    for (const tracker &t : torrent->get_trackers())
    {
        tracker_status &ts = torrent->get_tracker_status(t.url());

        // TODO: std::find_if uses moderate CPU here, and in a loop as well. maybe a std::map is better for peers_.
        auto &item = std::find_if(trackers_.begin(), trackers_.end(), [t](const tracker_state &ts) { return t.url() == ts.tracker.url(); });

        if (item != trackers_.end())
        {
            item->status = ts;
            item->tracker = t;
            item->dirty = true;
        }
        else
        {
            tracker_state state(t);
            state.dirty = true;
            state.status = ts;
            trackers_.push_back(state);
        }
    }

    trackers_.erase(std::remove_if(trackers_.begin(), trackers_.end(),
        [](const tracker_state &t)
    {
        return !t.dirty;
    }), trackers_.end());

    list_->set_item_count((int)trackers_.size());
}

void trackers_page::on_init_dialog()
{
    HWND hList = GetDlgItem(handle(), ID_DETAILS_TRACKERS_LIST);
    list_ = std::make_unique<list_view>(hList);

    list_->add_column(LIST_COLUMN_URL,    TR("url"),           scaler::x(240));
    list_->add_column(LIST_COLUMN_STATUS, TR("status"),        scaler::x(100));
    list_->add_column(LIST_COLUMN_UPDATE, TR("next_announce"), scaler::x(100), list_view::number);
    list_->add_column(LIST_COLUMN_PEERS,  TR("peers"),         scaler::x(80),  list_view::number);
    list_->add_column(LIST_COLUMN_SCRAPE, TR("scrape"),        scaler::x(80),  list_view::number);

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
    case LIST_COLUMN_UPDATE:
    {
        std::chrono::seconds next = t.tracker.next_announce_in();

        if (next.count() < 0)
        {
            return L"-";
        }

        std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(next);
        std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(next - min_left);

        // Return unknown if more than 60 minutes
        if (min_left.count() >= 60)
        {
            return L"-";
        }

        TCHAR t[100];
        StringCchPrintf(
            t,
            ARRAYSIZE(t),
            L"%dm %ds",
            min_left.count(),
            sec_left.count());
        return t;
    }
    case LIST_COLUMN_PEERS:
        if (t.status.num_peers < 0)
        {
            return L"-";
        }

        return std::to_wstring(t.status.num_peers);
    case LIST_COLUMN_SCRAPE:
    {
        int complete = t.status.scrape_complete;
        int incomplete = t.status.scrape_incomplete;

        TCHAR t[100];
        StringCchPrintf(
            t,
            ARRAYSIZE(t),
            L"%s/%s",
            complete < 0 ? L"-" : std::to_wstring(complete).c_str(),
            incomplete < 0 ? L"-" : std::to_wstring(incomplete).c_str());
        return t;
    }
    default:
        return L"<unknown column>";
    }
}
