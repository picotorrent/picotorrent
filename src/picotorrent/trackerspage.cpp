#include "trackerspage.hpp"

#include "trackersviewmodel.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/dataview.h>

namespace lt = libtorrent;
using pt::TrackersPage;

TrackersPage::TrackersPage(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id),
	m_trackersView(new wxDataViewCtrl(this, wxID_ANY)),
	m_viewModel(new TrackersViewModel())
{
	auto urlCol = m_trackersView->AppendTextColumn("Url", 0, wxDATAVIEW_CELL_INERT, 220);
	m_trackersView->AppendTextColumn("Fails", 1, wxDATAVIEW_CELL_INERT, 60, wxALIGN_RIGHT);
	m_trackersView->AppendTextColumn("Verified", 2, wxDATAVIEW_CELL_INERT, 60, wxALIGN_RIGHT);
	m_trackersView->AppendTextColumn("Next announce", 3, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
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
