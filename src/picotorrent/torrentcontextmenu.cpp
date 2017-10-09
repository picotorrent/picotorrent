#include "torrentcontextmenu.hpp"

#include "sessionstate.hpp"

#include <libtorrent/torrent_handle.hpp>

namespace lt = libtorrent;
using pt::TorrentContextMenu;

BEGIN_EVENT_TABLE(TorrentContextMenu, wxMenu)
	EVT_MENU(ptID_QUEUE_UP, TorrentContextMenu::QueueUp)
END_EVENT_TABLE()

TorrentContextMenu::TorrentContextMenu(std::shared_ptr<pt::SessionState> state)
	: m_state(state)
{
	wxMenu* queueMenu = new wxMenu();
	queueMenu->Append(ptID_QUEUE_UP, "Up");
	queueMenu->Append(ptID_QUEUE_DOWN, "Down");
	queueMenu->AppendSeparator();
	queueMenu->Append(ptID_QUEUE_TOP, "Top");
	queueMenu->Append(ptID_QUEUE_BOTTOM, "Bottom");

	Append(ptID_RESUME, "Resume");
	Append(ptID_PAUSE, "Pause");
	AppendSeparator();
	Append(ptID_MOVE, "Move");
	Append(ptID_REMOVE, "Remove");
	AppendSeparator();
	AppendSubMenu(queueMenu, "Queuing");
	AppendSeparator();
	Append(ptID_COPY_INFO_HASH, "Copy info hash");
	Append(ptID_OPEN_IN_EXPLORER, "Open in Explorer");
}

void TorrentContextMenu::QueueUp(wxCommandEvent& WXUNUSED(event))
{
	printf("");
}
