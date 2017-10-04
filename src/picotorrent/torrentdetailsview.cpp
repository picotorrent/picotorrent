#include "torrentdetailsview.hpp"

#include "filespage.hpp"
#include "overviewpage.hpp"

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/notebook.h>

namespace lt = libtorrent;
using pt::TorrentDetailsView;

TorrentDetailsView::TorrentDetailsView(wxWindow* parent)
	: wxPanel(parent),
	m_notebook(new wxNotebook(this, wxID_ANY)),
	m_overview(new OverviewPage(m_notebook, wxID_ANY)),
	m_files(new FilesPage(m_notebook, wxID_ANY))
{
	m_notebook->AddPage(m_overview, "Overview");
	m_notebook->AddPage(m_files, "Files");
	m_notebook->AddPage(new wxPanel(m_notebook, wxID_ANY), "Peers");
	m_notebook->AddPage(new wxPanel(m_notebook, wxID_ANY), "Trackers");

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);	
	sizer->Add(m_notebook, 1, wxEXPAND, 0);

	this->SetSizer(sizer);
}

wxSize TorrentDetailsView::GetMinSize() const
{
	return wxSize(400, 100);
}

void TorrentDetailsView::SetTorrent(lt::torrent_handle const& th)
{
	lt::torrent_status ts = th.status();

	m_overview->Update(ts);
	m_files->Update(ts);
}
