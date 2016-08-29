#include "TrackersPage.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/torrent_handle.hpp>

#include <strsafe.h>

#include "../../Commands/AddTrackerCommand.hpp"
#include "../../Commands/RemoveTrackersCommand.hpp"
#include "../../Configuration.hpp"
#include "../../Dialogs/AddTrackerDialog.hpp"
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

LRESULT TrackersPage::OnAddTracker(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto cmd = reinterpret_cast<Commands::AddTrackerCommand*>(lParam);

    if (!cmd->url.empty())
    {
        lt::announce_entry entry(TS(cmd->url));
        m_torrent.add_tracker(entry);

        // Add it to the list
        Models::Tracker t = Map(entry);
        m_trackerList->Add(t);
        m_trackerList->SetItemCount(m_trackerList->GetItemCount() + 1);
    }

    return FALSE;
}

LRESULT TrackersPage::OnRemoveTrackers(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto cmd = reinterpret_cast<Commands::RemoveTrackersCommand*>(lParam);

    if (cmd->trackers.size() > 0)
    {
        auto trackers = m_torrent.trackers();

        for (auto it = trackers.begin(); it != trackers.end();)
        {
            std::string url = it->url;

            auto f = std::find_if(
                cmd->trackers.begin(),
                cmd->trackers.end(),
                [url](const Models::Tracker& t) { return t.url == url; });
            
            if (f != cmd->trackers.end())
            {
                Models::Tracker t = Map(*it);
                m_trackerList->Remove(t);

                it = trackers.erase(it);
            }
            else
            {
                ++it;
            }
        }

        m_torrent.replace_trackers(trackers);
        m_trackerList->SetItemCount((int)trackers.size());
    }

    return FALSE;
}

BOOL TrackersPage::OnKillActive()
{
    m_trackerList->RemoveAll();
    m_trackerList->SetItemCount(0);

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
