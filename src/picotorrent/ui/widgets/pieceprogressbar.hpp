#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <libtorrent/bitfield.hpp>
#include <libtorrent/fwd.hpp>

namespace pt::UI::Widgets
{
    class PieceProgressBar : public wxPanel
    {
    public:
        PieceProgressBar(wxWindow* parent, wxWindowID id, libtorrent::typed_bitfield<libtorrent::piece_index_t> field = {});
        void UpdateBitfield(libtorrent::typed_bitfield<libtorrent::piece_index_t> const& field);

    protected:
        void OnEraseBackground(wxEraseEvent&);
        void OnSize(wxSizeEvent&);
        void OnPaint(wxPaintEvent&);

    private:
        void RenderProgress(wxDC& dc);

        libtorrent::typed_bitfield<libtorrent::piece_index_t> m_bitfield;
    };
}
