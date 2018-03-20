#include "overviewpage.hpp"

#include "translator.hpp"

#include <sstream>

#include <libtorrent/torrent_status.hpp>

namespace lt = libtorrent;
using pt::OverviewPage;

OverviewPage::OverviewPage(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, id),
    m_name(new wxStaticText(this, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END)),
    m_infoHash(new wxStaticText(this, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END)),
    m_savePath(new wxStaticText(this, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END)),
    m_pieces(new wxStaticText(this, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END))
{
    wxFlexGridSizer* sz = new wxFlexGridSizer(4, 10, 10);
    sz->AddGrowableCol(1);
    sz->AddGrowableCol(3);

    sz->Add(GetBoldStatic(i18n(tr, "name")));
    sz->Add(m_name, 1, wxEXPAND);
    sz->Add(GetBoldStatic(i18n(tr, "info_hash")));
    sz->Add(m_infoHash, 1, wxEXPAND);

    sz->Add(GetBoldStatic(i18n(tr, "save_path")));
    sz->Add(m_savePath, 1, wxEXPAND);
    sz->Add(GetBoldStatic(i18n(tr, "pieces")));
    sz->Add(m_pieces, 1, wxEXPAND);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(sz, 1, wxALL | wxEXPAND, 5);

    this->SetSizerAndFit(mainSizer);
}

void OverviewPage::Clear()
{
    m_name->SetLabel("-");
    m_infoHash->SetLabel("-");
    m_savePath->SetLabel("-");
    m_pieces->SetLabel("-");
}

void OverviewPage::Update(lt::torrent_status const& ts)
{
    std::stringstream ih;
    ih << ts.info_hash;

    m_name->SetLabel(ts.name);
    m_infoHash->SetLabel(ih.str());
    m_savePath->SetLabel(ts.save_path);
    m_pieces->SetLabel(wxString::Format("%d (of %d)", ts.pieces.count(), ts.pieces.size()));

    this->SendSizeEvent();
}

wxStaticText* OverviewPage::GetBoldStatic(wxString const& label)
{
    wxStaticText* ctrl = new wxStaticText(this, wxID_ANY, label);	
    wxFont font = ctrl->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    ctrl->SetFont(font);
    return ctrl;
}
