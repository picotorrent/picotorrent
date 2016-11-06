#pragma once

#include "../../resources.h"
#include "../../stdafx.h"

#include <memory>
#include <string>

namespace libtorrent
{
    struct announce_entry;
    struct torrent_handle;
}

namespace Models
{
    struct Tracker;
}

namespace UI
{
    class TrackerListView;
}

namespace PropertySheets
{
namespace Details
{
    class TrackersPage : public CPropertyPageImpl<TrackersPage>
    {
        friend class CPropertyPageImpl<TrackersPage>;

    public:
        enum { IDD = IDD_DETAILS_TRACKERS };
        TrackersPage(const libtorrent::torrent_handle& torrent);

    private:
        Models::Tracker Map(const libtorrent::announce_entry& entry);

        LRESULT OnAddTracker(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnRemoveTrackers(UINT uMsg, WPARAM wParam, LPARAM lParam);
        BOOL OnKillActive();
        BOOL OnSetActive();
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
        void OnTimer(UINT_PTR nIDEvent);

        BEGIN_MSG_MAP_EX(TrackersPage)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_TIMER(OnTimer)
            MESSAGE_HANDLER_EX(PT_ADDTRACKER, OnAddTracker)
            MESSAGE_HANDLER_EX(PT_REMOVETRACKERS, OnRemoveTrackers)
            CHAIN_MSG_MAP(CPropertyPageImpl<TrackersPage>)
        END_MSG_MAP()

        std::wstring m_title;
        const libtorrent::torrent_handle& m_torrent;
        std::unique_ptr<UI::TrackerListView> m_trackerList;
    };
}
}
