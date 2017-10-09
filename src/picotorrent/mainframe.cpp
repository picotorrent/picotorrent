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

#include "environment.hpp"
#include "mainmenu.hpp"
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
	EVT_TIMER(ptID_MAIN_TIMER, MainFrame::OnTimer)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(std::shared_ptr<pt::Environment> env)
	: wxFrame(NULL, wxID_ANY, "PicoTorrent"),
	m_env(env),
	m_splitter(new wxSplitterWindow(this, wxID_ANY)),
	m_torrentListViewModel(new TorrentListViewModel())
{
	m_state = SessionLoader::Load(m_env);
	m_state->session->set_alert_notify(
		[this]()
	{
		this->GetEventHandler()->CallAfter(std::bind(&MainFrame::OnSessionAlert, this));
	});

	// Create UI
	m_torrentListView = new TorrentListView(m_splitter, ptID_TORRENT_LIST_VIEW);
	m_torrentListView->AssociateModel(m_torrentListViewModel);
	m_torrentDetailsView = new TorrentDetailsView(m_splitter, m_state);

	// Splitter
	m_splitter->SetSashGravity(0.5);
	m_splitter->SplitHorizontally(m_torrentListView, m_torrentDetailsView);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(m_splitter, 1, wxEXPAND, 0);
	mainSizer->SetSizeHints(this);

	this->SetIcon(wxICON(AppIcon));
	this->SetMenuBar(new MainMenu(m_state));
	this->SetSizerAndFit(mainSizer);

	m_timer = new wxTimer(this, ptID_MAIN_TIMER);
	m_timer->Start(1000);
}

MainFrame::~MainFrame()
{
	m_state->session->set_alert_notify([]() {});
	SessionUnloader::Unload(m_state, m_env);
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

	m_torrentDetailsView->Update();
}

void MainFrame::OnTorrentContextMenu(wxDataViewEvent& event)
{
	if (m_state->selected_torrents.empty())
	{
		return;
	}

	TorrentContextMenu menu(m_state);
	PopupMenu(&menu);
}

void MainFrame::OnTorrentSelectionChanged(wxDataViewEvent& event)
{
	wxDataViewItemArray items;
	m_torrentListView->GetSelections(items);

	m_state->selected_torrents.clear();
	
	if (items.IsEmpty())
	{
		return;
	}

	for (wxDataViewItem& item : items)
	{
		unsigned int row = m_torrentListViewModel->GetRow(item);

		lt::sha1_hash hash = m_torrentListViewModel->FindHashByRow(row);
		lt::torrent_handle th = m_state->torrents.at(hash);

		m_state->selected_torrents.push_back(th);
	}

	m_torrentDetailsView->Update();
}
