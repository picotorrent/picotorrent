#include "torrentdetailsview.hpp"

#include <wx/notebook.h>

using pt::TorrentDetailsView;

TorrentDetailsView::TorrentDetailsView(wxWindow* parent)
	: wxPanel(parent),
	m_notebook(new wxNotebook(this, wxID_ANY))
{
	m_notebook->AddPage(new wxPanel(m_notebook, wxID_ANY), "Overview");
	m_notebook->AddPage(new wxPanel(m_notebook, wxID_ANY), "Files");
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
