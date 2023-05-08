#include "pieceprogressbar.hpp"

#include <wx/dcbuffer.h>

#include "../theming/theming.hpp"

namespace lt = libtorrent;
using pt::UI::Widgets::PieceProgressBar;

PieceProgressBar::PieceProgressBar(wxWindow* parent, wxWindowID id, lt::typed_bitfield<lt::piece_index_t> field)
    : wxPanel(parent, id, wxDefaultPosition, wxSize(-1, parent->FromDIP(15)), wxTAB_TRAVERSAL | wxNO_BORDER | wxBG_STYLE_PAINT),
    m_bitfield(field)
{
    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(PieceProgressBar::OnEraseBackground));
    Connect(wxEVT_PAINT, wxPaintEventHandler(PieceProgressBar::OnPaint));
    Connect(wxEVT_SIZE, wxSizeEventHandler(PieceProgressBar::OnSize));
}

void PieceProgressBar::UpdateBitfield(lt::typed_bitfield<lt::piece_index_t> const& field)
{
    m_bitfield = field;
    Refresh();
}

void PieceProgressBar::OnEraseBackground(wxEraseEvent&)
{
}

void PieceProgressBar::OnSize(wxSizeEvent&)
{
    Refresh();
}

void PieceProgressBar::OnPaint(wxPaintEvent&)
{
    wxBufferedPaintDC dc(this);
    RenderProgress(dc);
}

void PieceProgressBar::RenderProgress(wxDC& dc)
{
    static wxColor bar("#35b1e1");
    static wxColor darkBorder(50, 50, 50);

    if (m_bitfield.size() > 0)
    {
        wxBitmap prg(m_bitfield.size() + 2, this->GetClientSize().GetHeight());
        wxMemoryDC memDC;

        memDC.SelectObject(prg);
        if (pt::UI::Theming::GetInstance().GetMSWDarkMode())
        {
            memDC.SetBrush(wxColor(24, 24, 24));
        }
        else
        {
            memDC.SetBrush(*wxWHITE);
        }
        memDC.SetPen(darkBorder);
        memDC.DrawRectangle({ 0, 0 }, prg.GetSize());

        memDC.SetPen(bar);

        for (int idx = 0; idx < m_bitfield.size(); idx++)
        {
            lt::piece_index_t pcs{ idx };

            if (m_bitfield[pcs])
            {
                memDC.DrawLine(idx + 1, 1, idx + 1, prg.GetHeight() - 1);
            }
        }

        dc.StretchBlit(
            { 0, 0 },
            this->GetClientSize(),
            &memDC,
            { 0, 0 },
            prg.GetSize());
    }
    else
    {
        if (pt::UI::Theming::GetInstance().GetMSWDarkMode())
        {
            dc.SetBrush(wxColor(24, 24, 24));
        }
        else
        {
            dc.SetBrush(*wxWHITE);
        }
        dc.SetPen(wxColor(190, 190, 190));
        dc.DrawRectangle(this->GetClientRect());
    }
}
