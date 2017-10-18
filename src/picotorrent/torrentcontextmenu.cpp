#include "torrentcontextmenu.hpp"

#include "sessionstate.hpp"
#include "translator.hpp"

#include <sstream>

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <wx/clipbrd.h>
#include <wx/filedlg.h>

namespace lt = libtorrent;
using pt::TorrentContextMenu;

BEGIN_EVENT_TABLE(TorrentContextMenu, wxMenu)
	EVT_MENU(ptID_QUEUE_UP, TorrentContextMenu::QueueUp)
	EVT_MENU(ptID_QUEUE_DOWN, TorrentContextMenu::QueueDown)
	EVT_MENU(ptID_QUEUE_TOP, TorrentContextMenu::QueueTop)
	EVT_MENU(ptID_QUEUE_BOTTOM, TorrentContextMenu::QueueBottom)
	//EVT_MENU(ptID_RESUME)
	//EVT_MENU(ptID_PAUSE
	EVT_MENU(ptID_MOVE, TorrentContextMenu::Move)
	EVT_MENU(ptID_REMOVE, TorrentContextMenu::Remove)
	EVT_MENU(ptID_COPY_INFO_HASH, TorrentContextMenu::CopyInfoHash)
	EVT_MENU(ptID_OPEN_IN_EXPLORER, TorrentContextMenu::OpenInExplorer)
END_EVENT_TABLE()

TorrentContextMenu::TorrentContextMenu(
	wxWindow* parent,
	std::shared_ptr<pt::Translator> tr,
	std::shared_ptr<pt::SessionState> state)
	: m_state(state),
	m_trans(tr),
	m_parent(parent)
{
	wxMenu* queueMenu = new wxMenu();
	queueMenu->Append(ptID_QUEUE_UP, i18n(tr, "up"));
	queueMenu->Append(ptID_QUEUE_DOWN, i18n(tr, "down"));
	queueMenu->AppendSeparator();
	queueMenu->Append(ptID_QUEUE_TOP, i18n(tr, "top"));
	queueMenu->Append(ptID_QUEUE_BOTTOM, i18n(tr, "bottom"));

	Append(ptID_RESUME, i18n(tr, "resume"));
	Append(ptID_PAUSE, i18n(tr, "pause"));
	AppendSeparator();
	Append(ptID_MOVE, i18n(tr, "move"));
	Append(ptID_REMOVE, i18n(tr, "remove"));
	AppendSeparator();
	AppendSubMenu(queueMenu, i18n(tr, "queuing"));
	AppendSeparator();
	Append(ptID_COPY_INFO_HASH, i18n(tr, "copy_info_hash"));
	Append(ptID_OPEN_IN_EXPLORER, i18n(tr, "open_in_explorer"));
}

void TorrentContextMenu::CopyInfoHash(wxCommandEvent& WXUNUSED(event))
{
	std::stringstream ss;

	for (lt::torrent_handle& th : m_state->selected_torrents)
	{
		ss << "," << th.info_hash();
	}

	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(ss.str().substr(1)));
		wxTheClipboard->Close();
	}
}

void TorrentContextMenu::Move(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(
		m_parent,
		i18n(m_trans, "select_destination"),
		wxEmptyString,
		wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() != wxID_OK)
	{
		return;
	}

	for (lt::torrent_handle& th : m_state->selected_torrents)
	{
		th.move_storage(dlg.GetPath().ToStdString());
	}
}

void TorrentContextMenu::OpenInExplorer(wxCommandEvent& WXUNUSED(event))
{
}

void TorrentContextMenu::Remove(wxCommandEvent& WXUNUSED(event))
{
	// TODO: ask if user wants to remove data files

	for (lt::torrent_handle& th : m_state->selected_torrents)
	{
		m_state->session->remove_torrent(th);
	}
}

void TorrentContextMenu::QueueUp(wxCommandEvent& WXUNUSED(event))
{
	for (lt::torrent_handle& th : m_state->selected_torrents)
	{
		th.queue_position_up();
	}
}

void TorrentContextMenu::QueueDown(wxCommandEvent& WXUNUSED(event))
{
	for (lt::torrent_handle& th : m_state->selected_torrents)
	{
		th.queue_position_down();
	}
}

void TorrentContextMenu::QueueTop(wxCommandEvent& WXUNUSED(event))
{
	for (lt::torrent_handle& th : m_state->selected_torrents)
	{
		th.queue_position_top();
	}
}

void TorrentContextMenu::QueueBottom(wxCommandEvent& WXUNUSED(event))
{
	for (lt::torrent_handle& th : m_state->selected_torrents)
	{
		th.queue_position_bottom();
	}
}
