#include "mainframe.hpp"

#include <libtorrent/session.hpp>

#include <wx/aboutdlg.h>
#include <wx/dataview.h>
#include <wx/notebook.h>
#include <wx/splitter.h>

#include "torrentdetailsview.hpp"
#include "torrentlistview.hpp"

using pt::MainFrame;

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
	EVT_MENU(wxID_EXIT, MainFrame::OnExit)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
	: wxFrame(NULL, wxID_ANY, "PicoTorrent"),
	m_splitter(new wxSplitterWindow(this, wxID_ANY)),
	m_torrentListView(new TorrentListView(m_splitter)),
	m_torrentDetailsView(new TorrentDetailsView(m_splitter)),
	m_session(std::make_shared<libtorrent::session>())
{
	wxMenu* menuFile = new wxMenu();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuHelp = new wxMenu();
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	m_splitter->SetSashGravity(0.5);
	m_splitter->SplitHorizontally(m_torrentListView, m_torrentDetailsView);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(m_splitter, 1, wxEXPAND, 0);
	mainSizer->SetSizeHints(this);

	this->SetIcon(wxICON(AppIcon));
	this->SetSizerAndFit(mainSizer);
}

MainFrame::~MainFrame()
{
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo aboutInfo;
	aboutInfo.SetName("PicoTorrent");
	aboutInfo.SetVersion("1.0");
	aboutInfo.SetDescription(_("Built with love on top of Boost, OpenSSL and Rasterbar-libtorrent."));
	aboutInfo.SetCopyright("(C) 2015-2017");
	aboutInfo.SetWebSite("http://picotorrent.org");
	aboutInfo.AddDeveloper("Viktor Elofsson");

	wxAboutBox(aboutInfo);
}

void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}
