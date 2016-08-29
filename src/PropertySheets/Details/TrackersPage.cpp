#include "TrackersPage.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/torrent_handle.hpp>

#include <strsafe.h>

#include "../../Configuration.hpp"
#include "../../Models/Tracker.hpp"
#include "../../resources.h"
#include "../../Translator.hpp"
#include "../../UI/TrackerListView.hpp"

#define REFRESH_TIMER WM_USER+1337
#define REFRESH_INTERVAL 900

namespace lt = libtorrent;
using PropertySheets::Details::TrackersPage;

Models::Tracker::State GetTrackerState(const lt::announce_entry& ae)
{
    if (ae.verified && ae.is_working())
    {
        return Models::Tracker::State::Working;
    }
    else if (ae.fails == 0 && ae.updating)
    {
        return Models::Tracker::State::Updating;
    }
    else if (ae.fails == 0)
    {
        return Models::Tracker::State::Unknown;
    }

    return Models::Tracker::State::NotWorking;
}

TrackersPage::TrackersPage(const lt::torrent_handle& torrent)
    : m_torrent(torrent)
{
    m_title = TRW("trackers");
    SetTitle(m_title.c_str());
}

Models::Tracker TrackersPage::Map(const lt::announce_entry& entry)
{
    Models::Tracker t;
    t.url = entry.url;
    t.state = GetTrackerState(entry);
    t.nextUpdate = std::chrono::seconds(entry.next_announce_in());
    t.complete = entry.scrape_complete;
    t.downloaded = entry.scrape_downloaded;
    t.incomplete = entry.scrape_incomplete;

    return t;
}

BOOL TrackersPage::OnKillActive()
{
    KillTimer(REFRESH_TIMER);
    return TRUE;
}

BOOL TrackersPage::OnSetActive()
{
    auto trackers = m_torrent.trackers();

    for (lt::announce_entry& tr : trackers)
    {
        Models::Tracker t = Map(tr);
        m_trackerList->Add(t);
    }

    m_trackerList->SetItemCount((int)trackers.size());

    // Refresh UI every 900ms to avoid skipping seconds
    // in the update timer.
    SetTimer(REFRESH_TIMER, REFRESH_INTERVAL);

    return TRUE;
}

BOOL TrackersPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    // Set up UI
    m_trackerList = std::make_unique<UI::TrackerListView>(GetDlgItem(ID_DETAILS_TRACKERS_LIST));
    return FALSE;
}

void TrackersPage::OnTimer(UINT_PTR nIDEvent)
{
    auto trackers = m_torrent.trackers();

    for (lt::announce_entry& tr : trackers)
    {
        Models::Tracker t = Map(tr);
        m_trackerList->Update(t);
    }

    auto sel = m_trackerList->GetVisibleIndices();
    m_trackerList->RedrawItems(sel.first, sel.second);
}
