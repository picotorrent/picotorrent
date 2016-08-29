#include "TrackerListView.hpp"

#include <strsafe.h>

#include "../Models/Tracker.hpp"
#include "../Scaler.hpp"
#include "../Translator.hpp"

#define CTX_MENU_ADD 1
#define CTX_MENU_REMOVE 2

#define LV_COL_URL    1
#define LV_COL_STATUS 2
#define LV_COL_UPDATE 3
#define LV_COL_SCRAPE 4

using UI::TrackerListView;

TrackerListView::TrackerListView(HWND hWndParent)
    : ListView::ListView(hWndParent)
{
    AddColumn(LV_COL_URL, TRW("url"), SX(240), ColumnType::Text);
    AddColumn(LV_COL_STATUS, TRW("status"), SX(100), ColumnType::Text);
    AddColumn(LV_COL_UPDATE, TRW("next_announce"), SX(100), ColumnType::Number);
    AddColumn(LV_COL_SCRAPE, TRW("scrape"), SX(120), ColumnType::Number);
}

TrackerListView::~TrackerListView()
{
}

void TrackerListView::Add(const Models::Tracker& tracker)
{
    m_trackers.push_back(tracker);
}

void TrackerListView::Update(const Models::Tracker& tracker)
{
    auto f = std::find_if(m_trackers.begin(), m_trackers.end(), [tracker](const Models::Tracker& t) { return t.url == tracker.url; });

    if (f != m_trackers.end())
    {
        auto index = std::distance(m_trackers.begin(), f);
        m_trackers.at(index) = tracker;
    }
}

std::wstring TrackerListView::GetItemText(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_URL:
        return TWS(m_trackers.at(itemIndex).url);
    case LV_COL_STATUS:
    {
        switch (m_trackers.at(itemIndex).state)
        {
        case Models::Tracker::State::NotWorking:
            return TRW("not_working");
        case Models::Tracker::State::Updating:
            return TRW("updating");
        case Models::Tracker::State::Working:
            return TRW("working");
        case Models::Tracker::State::Unknown:
        default:
            return TRW("unknown");
        }
    }
    case LV_COL_UPDATE:
    {
        std::chrono::seconds next = m_trackers.at(itemIndex).nextUpdate;
        if (next.count() < 0) { return L"-"; }

        std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(next);
        std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(next - min_left);

        // Return unknown if more than 60 minutes
        if (min_left.count() >= 60) { return L"-"; }

        TCHAR t[100];
        StringCchPrintf(t, ARRAYSIZE(t), L"%dm %ds", min_left.count(), sec_left.count());
        return t;
    }
    case LV_COL_SCRAPE:
    {
        int downloaded = m_trackers.at(itemIndex).downloaded;
        int complete = m_trackers.at(itemIndex).complete;
        int incomplete = m_trackers.at(itemIndex).incomplete;

        TCHAR t[100];
        StringCchPrintf(
            t,
            ARRAYSIZE(t),
            L"%d / %d / %d",
            downloaded,
            complete,
            incomplete);
        return t;
    }
    }

    return L"?unknown?";
}

void TrackerListView::ShowContextMenu(POINT p, const std::vector<int>& selectedIndices)
{
    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_STRING, CTX_MENU_ADD, TRW("add_tracker"));
    AppendMenu(menu, MF_STRING, CTX_MENU_REMOVE, TRW("remove_tracker_s"));

    if (selectedIndices.size() > 0)
    {
        EnableMenuItem(menu, CTX_MENU_ADD, MF_BYCOMMAND | MF_DISABLED);
    }
    else
    {
        EnableMenuItem(menu, CTX_MENU_REMOVE, MF_BYCOMMAND | MF_DISABLED);
    }

    int res = TrackPopupMenu(
        menu,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        0,
        m_hWnd,
        NULL);

    switch (res)
    {
    case CTX_MENU_ADD:
    {
        // Show AddTrackerDialog
        // send AddTrackerCommand
        break;
    }
    case CTX_MENU_REMOVE:
    {
        // send RemoveTrackerCommand
        break;
    }
    }
}
