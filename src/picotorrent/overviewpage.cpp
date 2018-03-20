#include "overviewpage.hpp"

#include "translator.hpp"

#include <libtorrent/torrent_status.hpp>
#include <sstream>
#include <wx/clipbrd.h>

namespace lt = libtorrent;
using pt::OverviewPage;

class CopyableStaticText : public wxStaticText
{
public:
    CopyableStaticText(wxWindow *parent, std::shared_ptr<pt::Translator> tr)
        : wxStaticText(parent, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END, wxStaticTextNameStr)
    {
        this->Bind(wxEVT_RIGHT_DOWN, [this, tr](wxMouseEvent const& ev)
        {
            if (this->GetLabel() == "-")
            {
                return;
            }

            wxMenu menu;
            menu.Append(9999, i18n(tr, "copy"));
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

OverviewPage::OverviewPage(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, id),
    m_name(new CopyableStaticText(this, tr)),
    m_infoHash(new CopyableStaticText(this, tr)),
    m_savePath(new CopyableStaticText(this, tr)),
    m_pieces(new CopyableStaticText(this, tr))
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
