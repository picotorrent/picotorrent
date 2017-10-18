#include "torrentcontextmenu.hpp"

#include "sessionstate.hpp"
#include "translator.hpp"

#include <libtorrent/torrent_handle.hpp>

namespace lt = libtorrent;
using pt::TorrentContextMenu;

BEGIN_EVENT_TABLE(TorrentContextMenu, wxMenu)
	EVT_MENU(ptID_QUEUE_UP, TorrentContextMenu::QueueUp)
END_EVENT_TABLE()

TorrentContextMenu::TorrentContextMenu(std::shared_ptr<pt::Translator> tr,
	std::shared_ptr<pt::SessionState> state)
	: m_state(state)
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

void TorrentContextMenu::QueueUp(wxCommandEvent& WXUNUSED(event))
{
	printf("");
}
