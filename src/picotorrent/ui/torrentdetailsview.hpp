#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <map>

#include <libtorrent/info_hash.hpp>
#include <wx/notebook.h>

namespace pt
{
namespace BitTorrent
{
    class TorrentHandle;
}
namespace UI
{
    class TorrentDetailsFilesPanel;
    class TorrentDetailsOverviewPanel;
    class TorrentDetailsPeersPanel;
    class TorrentDetailsTrackersPanel;

    class TorrentDetailsView : public wxNotebook
    {
    public:
        TorrentDetailsView(wxWindow* parent, wxWindowID id);
        virtual ~TorrentDetailsView();

        void Refresh(std::map<libtorrent::info_hash_t, BitTorrent::TorrentHandle*> const& torrents);
        void Reset();

    private:
        TorrentDetailsOverviewPanel* m_overview;
        TorrentDetailsFilesPanel* m_files;
        TorrentDetailsPeersPanel* m_peers;
        TorrentDetailsTrackersPanel* m_trackers;
    };
}
}
