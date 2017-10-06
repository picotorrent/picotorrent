#include "mainframe.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/write_resume_data.hpp>

#include <filesystem>
#include <fstream>
#include <limits>

#include <wx/aboutdlg.h>
#include <wx/dataview.h>
#include <wx/filedlg.h>
#include <wx/notebook.h>
#include <wx/splitter.h>

#include "addtorrentdlg.hpp"
#include "environment.hpp"
#include "sessionloader.hpp"
#include "sessionstate.hpp"
#include "sessionunloader.hpp"
#include "torrentcontextmenu.hpp"
#include "torrentdetailsview.hpp"
#include "torrentlistview.hpp"
#include "torrentlistviewmodel.hpp"

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using pt::MainFrame;

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(ptID_TORRENT_LIST_VIEW, MainFrame::OnTorrentContextMenu)
	EVT_DATAVIEW_SELECTION_CHANGED(ptID_TORRENT_LIST_VIEW, MainFrame::OnTorrentSelectionChanged)
	EVT_MENU(ptID_ADD_TORRENTS, MainFrame::OnAddTorrents)
	EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
	EVT_MENU(wxID_EXIT, MainFrame::OnExit)
	EVT_TIMER(ptID_MAIN_TIMER, MainFrame::OnTimer)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(std::shared_ptr<pt::Environment> env)
	: wxFrame(NULL, wxID_ANY, "PicoTorrent"),
	m_env(env),
	m_splitter(new wxSplitterWindow(this, wxID_ANY)),
	m_timer(new wxTimer(this, ptID_MAIN_TIMER)),
	m_torrentListView(new TorrentListView(m_splitter, ptID_TORRENT_LIST_VIEW)),
	m_torrentListViewModel(new TorrentListViewModel()),
	m_torrentDetailsView(new TorrentDetailsView(m_splitter))
{
	m_torrentListView->AssociateModel(m_torrentListViewModel);

	wxMenu* menuFile = new wxMenu();
	menuFile->Append(ptID_ADD_TORRENTS, "Add torrent(s)");
	menuFile->AppendSeparator();
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

	m_state = SessionLoader::Load(m_env);
	m_state->session->set_alert_notify(
		[this]()
	{
		this->GetEventHandler()->CallAfter(std::bind(&MainFrame::OnSessionAlert, this));
	});

	m_timer->Start(1000);
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

void MainFrame::OnAddTorrents(wxCommandEvent& WXUNUSED(event))
{
	// Open some torrent files!
	wxFileDialog openDialog(
		this,
		"Add torrent files",
		wxEmptyString,
		wxEmptyString,
		"Torrent files (*.torrent)|*.torrent",
		wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

	if (openDialog.ShowModal() != wxID_OK)
	{
		return;
	}

	wxArrayString paths;
	openDialog.GetPaths(paths);

	if (paths.IsEmpty())
	{
		return;
	}

	std::vector<lt::add_torrent_params> params;

	for (wxString& filePath : paths)
	{
		lt::add_torrent_params p;
		lt::error_code ec;

		// save path
		p.ti = std::make_shared<lt::torrent_info>(filePath.ToStdString(), ec);

		if (ec)
		{
			continue;
		}

		params.push_back(p);
	}

	AddTorrentDialog addDialog(this, params);
	int result = addDialog.ShowModal();

	if (result == wxID_OK)
	{
		for (lt::add_torrent_params& p : params)
		{
			m_state->session->async_add_torrent(p);
		}
	}
}

void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	m_state->session->set_alert_notify([]() {});
	SessionUnloader::Unload(m_state, m_env);

	Close(true);
}

void MainFrame::OnSessionAlert()
{
	std::vector<lt::alert*> alerts;
	m_state->session->pop_alerts(&alerts);

	for (lt::alert* alert : alerts)
	{
		wxLogDebug("%s", alert->message().c_str());

		switch (alert->type())
		{
		case lt::add_torrent_alert::alert_type:
		{
			lt::add_torrent_alert* ata = lt::alert_cast<lt::add_torrent_alert>(alert);

			if (ata->error)
			{
				// TODO (logging)
				break;
			}

			if (ata->handle.torrent_file())
			{
				fs::path torrentsDirectory = m_env->GetApplicationDataPath() / "Torrents";
				if (!fs::exists(torrentsDirectory)) { fs::create_directories(torrentsDirectory); }

				lt::create_torrent ct(*ata->handle.torrent_file());
				lt::entry entry = ct.generate();

				std::stringstream hex;
				hex << ata->handle.info_hash();

				fs::path torrentFile = torrentsDirectory / (hex.str() + ".torrent");
				std::ofstream out(torrentFile, std::ios::binary | std::ios::out);
				lt::bencode(std::ostreambuf_iterator<char>(out), entry);

				// Generate a save resume data alert to save torrent state
				ata->handle.save_resume_data();
			}

			m_state->torrents.insert({ ata->handle.info_hash(), ata->handle });
			m_torrentListViewModel->Add(ata->handle.status());

			break;
		}
		case lt::save_resume_data_alert::alert_type:
		{
			lt::save_resume_data_alert* srda = lt::alert_cast<lt::save_resume_data_alert>(alert);

			fs::path torrentsDirectory = m_env->GetApplicationDataPath() / "Torrents";
			if (!fs::exists(torrentsDirectory)) { fs::create_directories(torrentsDirectory); }

			std::stringstream hex;
			hex << srda->handle.info_hash();

			lt::entry entry = lt::write_resume_data(srda->params);
			std::vector<char> buf;
			lt::bencode(std::back_inserter(buf), entry);

			fs::path datFile = torrentsDirectory / (hex.str() + ".dat");
			std::ofstream out(datFile, std::ios::binary | std::ios::out);
			std::copy(
				buf.begin(),
				buf.end(),
				std::ostreambuf_iterator<char>(out));

			break;
		}
		case lt::state_update_alert::alert_type:
		{
			lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);

			for (lt::torrent_status const& ts : sua->status)
			{
				m_torrentListViewModel->Update(ts);

				if (ts.info_hash == m_state->selected_torrent)
				{
					m_torrentDetailsView->Update(ts.handle);
				}
			}

			break;
		}
		}
	}
}

void MainFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	m_state->session->post_dht_stats();
	m_state->session->post_session_stats();
	m_state->session->post_torrent_updates();
}

void MainFrame::OnTorrentContextMenu(wxDataViewEvent& event)
{
	const unsigned int MaxRow = std::numeric_limits<unsigned int>::max();

	unsigned int row = m_torrentListViewModel->GetRow(event.GetItem());

	if (row >= MaxRow)
	{
		return;
	}

	lt::sha1_hash hash = m_torrentListViewModel->FindHashByRow(row);
	lt::torrent_handle torrent = m_state->torrents.at(hash);

	TorrentContextMenu menu(torrent);
	PopupMenu(&menu);
}

void MainFrame::OnTorrentSelectionChanged(wxDataViewEvent& event)
{
	const unsigned int MaxRow = std::numeric_limits<unsigned int>::max();

	unsigned int row = m_torrentListViewModel->GetRow(event.GetItem());

	m_torrentDetailsView->Clear();

	if (row >= MaxRow)
	{
		m_state->selected_torrent = lt::sha1_hash();
		return;
	}

	m_state->selected_torrent = m_torrentListViewModel->FindHashByRow(row);
	lt::torrent_handle torrent = m_state->torrents.at(m_state->selected_torrent);

	m_torrentDetailsView->Update(torrent);
}
