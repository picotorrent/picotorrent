#include "peerspage.hpp"

#include "peersviewmodel.hpp"

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/dataview.h>

namespace lt = libtorrent;
using pt::PeersPage;

PeersPage::PeersPage(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id),
	m_peersView(new wxDataViewCtrl(this, wxID_ANY)),
	m_viewModel(new PeersViewModel())
{
	m_peersView->AppendTextColumn("Endpoint", 0, wxDATAVIEW_CELL_INERT, 120);
	m_peersView->AppendTextColumn("Client", 1, wxDATAVIEW_CELL_INERT, 120);
	m_peersView->AssociateModel(m_viewModel);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_peersView, 1, wxEXPAND);

	this->SetSizer(sizer);
}

void PeersPage::Update(lt::torrent_status const& ts)
{
	m_viewModel->Update(ts);
}
