#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt
{
namespace BitTorrent
{
    class TorrentHandle;
}
namespace UI
{
    class TorrentDetailsOverviewPanel : public wxPanel
    {
    public:
        TorrentDetailsOverviewPanel(wxWindow* parent, wxWindowID id);

        void Refresh(BitTorrent::TorrentHandle* torrent);
        void Reset();

    private:
        wxStaticText* m_name;
        wxStaticText* m_infoHash;
        wxStaticText* m_savePath;
        wxStaticText* m_pieces;
    };
}
}
