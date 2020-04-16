#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class wxDataViewCtrl;
class wxDataViewEvent;

namespace pt
{
namespace BitTorrent
{
    class TorrentHandle;
}
namespace UI
{
namespace Models
{
    class TrackerListModel;
}
    class TorrentDetailsTrackersPanel : public wxPanel
    {
    public:
        TorrentDetailsTrackersPanel(wxWindow* parent, wxWindowID id);

        void Refresh(BitTorrent::TorrentHandle* torrent);
        void Reset();

    private:
        enum
        {
            ptID_CONTEXT_MENU_REMOVE_TIER = wxID_HIGHEST + 1,
            ptID_CONTEXT_MENU_REMOVE_TRACKER,
            ptID_CONTEXT_MENU_COPY_URL,
            ptID_CONTEXT_MENU_FORCE_REANNOUNCE,
            ptID_CONTEXT_MENU_SCRAPE,
        };

        void ShowTrackerContextMenu(wxDataViewEvent&);

        BitTorrent::TorrentHandle* m_torrent;

        Models::TrackerListModel* m_trackersModel;
        wxDataViewCtrl* m_trackersView;
    };
}
}
