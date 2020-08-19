#include "torrentdetailsoverviewpanel.hpp"

#include <fmt/format.h>
#include <wx/clipbrd.h>
#include <wx/sizer.h>

#include "../bittorrent/torrenthandle.hpp"
#include "../bittorrent/torrentstatus.hpp"
#include "translator.hpp"

using pt::UI::TorrentDetailsOverviewPanel;

static wxStaticText* BoldLabel(wxWindow* parent, wxWindowID id, wxString const& text)
{
    auto s = new wxStaticText(parent, id, text);
    auto f = s->GetFont();
    f.SetWeight(wxFONTWEIGHT_BOLD);
    s->SetFont(f);
    return s;
}

class CopyableStaticText : public wxStaticText
{
public:
    CopyableStaticText(wxWindow* parent)
        : wxStaticText(parent, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END)
    {
        this->Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent const& ev)
            {
                if (this->GetLabel() == "-")
                {
                    return;
                }

                wxMenu menu;
                menu.Append(9999, i18n("copy"));
                menu.Bind(wxEVT_MENU, [this](wxCommandEvent const&)
                    {
                        if (wxTheClipboard->Open())
                        {
                            wxTheClipboard->SetData(new wxTextDataObject(this->GetLabel()));
                            wxTheClipboard->Close();
                        }
                    });

                PopupMenu(&menu);
            });
    }
};

TorrentDetailsOverviewPanel::TorrentDetailsOverviewPanel(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id),
    m_name(new CopyableStaticText(this)),
    m_infoHash(new CopyableStaticText(this)),
    m_savePath(new CopyableStaticText(this)),
    m_pieces(new CopyableStaticText(this))
{
    auto sizer = new wxFlexGridSizer(4, FromDIP(10), FromDIP(10));
    sizer->AddGrowableCol(1, 1);
    sizer->AddGrowableCol(3, 1);

    sizer->Add(BoldLabel(this, wxID_ANY, i18n("name")));
    sizer->Add(m_name, 1, wxEXPAND);
    sizer->Add(BoldLabel(this, wxID_ANY, i18n("info_hash")));
    sizer->Add(m_infoHash, 1, wxEXPAND);

    sizer->Add(BoldLabel(this, wxID_ANY, i18n("save_path")));
    sizer->Add(m_savePath, 1, wxEXPAND);
    sizer->Add(BoldLabel(this, wxID_ANY, i18n("pieces")));
    sizer->Add(m_pieces, 1, wxEXPAND);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(sizer, 1, wxALL | wxEXPAND, FromDIP(5));

    this->SetSizerAndFit(mainSizer);
}

void TorrentDetailsOverviewPanel::Refresh(BitTorrent::TorrentHandle* torrent)
{
    auto status = torrent->Status();

    m_name->SetLabel(status.name);
    m_savePath->SetLabel(status.savePath);
    m_infoHash->SetLabel(status.infoHash);
    m_pieces->SetLabel(
        fmt::format(
            i18n("d_of_d"),
            status.pieces.count(),
            status.pieces.size()));

    this->Layout();
}

void TorrentDetailsOverviewPanel::Reset()
{
    m_name->SetLabel("-");
    m_infoHash->SetLabel("-");
    m_savePath->SetLabel("-");
    m_pieces->SetLabel("-");
}
