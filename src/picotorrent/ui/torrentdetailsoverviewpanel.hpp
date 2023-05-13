#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "../core/configuration.hpp"

class wxFlexGridSizer;

namespace pt::UI::Widgets { class PieceProgressBar; }

namespace pt
{
namespace BitTorrent
{
    class TorrentHandle;
}
namespace UI
{
    class TorrentDetailsOverviewPanel : public wxScrolledWindow
    {
    public:
        TorrentDetailsOverviewPanel(wxWindow* parent, wxWindowID id, bool isDarkMode, int cols = 2, bool showPieceProgress = true);

        void Refresh(BitTorrent::TorrentHandle* torrent);
        void Reset();
        void UpdateView(int cols, bool showPieceProgress);

    private:
        wxFlexGridSizer* m_sizer;
        wxBoxSizer* m_mainSizer;
        Widgets::PieceProgressBar* m_pieceProgress;
        wxStaticText* m_name;
        wxStaticText* m_infoHash;
        wxStaticText* m_savePath;
        wxStaticText* m_pieces;
        wxStaticText* m_comment;
        wxStaticText* m_size;
        wxStaticText* m_priv;
        wxStaticText* m_ratio;
        wxStaticText* m_lastDownload;
        wxStaticText* m_lastUpload;
        wxStaticText* m_totalDownload;
        wxStaticText* m_totalUpload;
        bool m_isDarkMode;
    };
}
}
