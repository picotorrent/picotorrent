#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace libtorrent
{
	struct torrent_handle;
}

namespace pt
{
    class TorrentContextMenu : public wxMenu
    {
    public:
        TorrentContextMenu(libtorrent::torrent_handle const& th);

	private:
		void OnCommand(wxCommandEvent&);
    };
}
