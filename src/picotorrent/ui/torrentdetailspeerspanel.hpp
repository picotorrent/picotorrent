#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class wxDataViewCtrl;

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
    class PeerListModel;
}
    class TorrentDetailsPeersPanel : public wxPanel
    {
    public:
        TorrentDetailsPeersPanel(wxWindow* parent, wxWindowID id);

        void Refresh(BitTorrent::TorrentHandle* torrent);
        void Reset();

    private:
        BitTorrent::TorrentHandle* m_torrent;
        Models::PeerListModel* m_peersModel;
        wxDataViewCtrl* m_peersView;
    };
}
}
