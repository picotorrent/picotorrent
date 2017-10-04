#include "torrentcontextmenu.hpp"

#include <libtorrent/torrent_handle.hpp>

namespace lt = libtorrent;
using pt::TorrentContextMenu;

TorrentContextMenu::TorrentContextMenu(lt::torrent_handle const& th)
{
	wxMenu* queueMenu = new wxMenu();
	queueMenu->Append(wxID_ANY, "Up");
	queueMenu->Append(wxID_ANY, "Down");
	queueMenu->AppendSeparator();
	queueMenu->Append(wxID_ANY, "Top");
	queueMenu->Append(wxID_ANY, "Bottom");

	Append(wxID_ANY, "Resume");
	AppendSeparator();
	Append(wxID_ANY, "Move");
	Append(wxID_ANY, "Remove");
	AppendSeparator();
	AppendSubMenu(queueMenu, "Queuing");
	AppendSeparator();
	Append(wxID_ANY, "Copy info hash");
	Append(wxID_ANY, "Open in Explorer");

	Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TorrentContextMenu::OnCommand), NULL, this);
}

void TorrentContextMenu::OnCommand(wxCommandEvent& event)
{
	// TODO implement this
}
