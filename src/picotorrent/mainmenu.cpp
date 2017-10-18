#include "mainmenu.hpp"

#include "addtorrentdlg.hpp"
#include "addtorrentproc.hpp"
#include "preferencesdlg.hpp"
#include "sessionstate.hpp"
#include "translator.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/aboutdlg.h>

using pt::MainMenu;

wxBEGIN_EVENT_TABLE(MainMenu, wxMenuBar)
	EVT_MENU(wxID_ABOUT, MainMenu::OnAbout)
	EVT_MENU(ptID_ADD_TORRENTS, MainMenu::OnAddTorrents)
	EVT_MENU(ptID_VIEW_PREFERENCES, MainMenu::OnViewPreferences)
	EVT_MENU(wxID_EXIT, MainMenu::OnExit)
wxEND_EVENT_TABLE()

MainMenu::MainMenu(std::shared_ptr<pt::SessionState> state,
	std::shared_ptr<pt::Translator> translator)
	: wxMenuBar(),
	m_state(state),
	m_trans(translator)
{
	wxMenu* menuFile = new wxMenu();
	menuFile->Append(ptID_ADD_TORRENTS, m_trans->Translate("amp_add_torrent"));
	menuFile->Append(wxID_ANY, m_trans->Translate("amp_add_magnet_link_s"));
	menuFile->AppendSeparator();
	menuFile->Append(wxID_ANY, m_trans->Translate("amp_create_torrent"));
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuView = new wxMenu();
	menuView->Append(ptID_VIEW_PREFERENCES, m_trans->Translate("amp_preferences"));

	wxMenu* menuHelp = new wxMenu();
	menuHelp->Append(wxID_ABOUT);

	Append(menuFile, m_trans->Translate("amp_file"));
	Append(menuView, m_trans->Translate("amp_view"));
	Append(menuHelp, m_trans->Translate("amp_help"));
}

void MainMenu::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo aboutInfo;
	aboutInfo.SetName("PicoTorrent");
	aboutInfo.SetVersion("1.0");
	aboutInfo.SetDescription(m_trans->Translate("picotorrent_description"));
	aboutInfo.SetCopyright("(C) 2015-2017");
	aboutInfo.SetWebSite("http://picotorrent.org");
	aboutInfo.AddDeveloper("Viktor Elofsson");

	wxAboutBox(aboutInfo, this->GetFrame());
}

void MainMenu::OnAddTorrents(wxCommandEvent& event)
{
	AddTorrentProcedure proc(this->GetFrame(), m_trans, m_state);
	proc.Execute();
}

void MainMenu::OnExit(wxCommandEvent& WXUNUSED(event))
{
	this->GetFrame()->Close();
}

void MainMenu::OnViewPreferences(wxCommandEvent& WXUNUSED(event))
{
	PreferencesDialog dlg(this->GetFrame(), m_trans);
	dlg.ShowModal();
}
