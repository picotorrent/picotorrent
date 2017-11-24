#include "trackerspage.hpp"

#include "trackersviewmodel.hpp"
#include "translator.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/dataview.h>

namespace lt = libtorrent;
using pt::TrackersPage;

TrackersPage::TrackersPage(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, id),
    m_trackersView(new wxDataViewCtrl(this, wxID_ANY)),
    m_viewModel(new TrackersViewModel())
{
    auto urlCol = m_trackersView->AppendTextColumn(i18n(tr, "url"), 0, wxDATAVIEW_CELL_INERT, 220);
    m_trackersView->AppendTextColumn(i18n(tr, "fails"), 1, wxDATAVIEW_CELL_INERT, 60, wxALIGN_RIGHT);
    m_trackersView->AppendTextColumn(i18n(tr, "verified"), 2, wxDATAVIEW_CELL_INERT, 60, wxALIGN_RIGHT);
    m_trackersView->AppendTextColumn(i18n(tr, "next_announce"), 3, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
    m_trackersView->AssociateModel(m_viewModel);

    urlCol->GetRenderer()->EnableEllipsize(wxELLIPSIZE_END);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_trackersView, 1, wxEXPAND);

    this->SetSizer(sizer);
}

void TrackersPage::Update(lt::torrent_status const& ts)
{
    m_viewModel->Update(ts);
}
