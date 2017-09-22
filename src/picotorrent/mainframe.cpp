#include "mainframe.hpp"

#include <wx/dataview.h>
#include <wx/notebook.h>
#include <wx/splitter.h>

#include "torrentdetailsview.hpp"
#include "torrentlistview.hpp"

using pt::MainFrame;

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
	: wxFrame(NULL, wxID_ANY, "PicoTorrent"),
	m_splitter(new wxSplitterWindow(this, wxID_ANY)),
	m_torrentListView(new TorrentListView(m_splitter)),
	m_torrentDetailsView(new TorrentDetailsView(m_splitter))
{
	wxMenu* menuFile = new wxMenu();
	menuFile->Append(wxID_EXIT);

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "&File");

	SetMenuBar(menuBar);

	m_splitter->SetSashGravity(0.5);
	m_splitter->SplitHorizontally(m_torrentListView,m_torrentDetailsView);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(m_splitter, 1, wxEXPAND, 0);
	mainSizer->SetSizeHints(this);

	this->SetSizerAndFit(mainSizer);
}
