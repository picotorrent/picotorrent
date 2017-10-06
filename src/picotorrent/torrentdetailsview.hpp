#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class wxNotebook;

namespace libtorrent
{
	struct torrent_handle;
}

namespace pt
{
	class FilesPage;
	class OverviewPage;
	class PeersPage;
	class TrackersPage;

    class TorrentDetailsView : public wxPanel
    {
    public:
        TorrentDetailsView(wxWindow* parent);
		virtual wxSize GetMinSize() const;

		void Clear();
		void Update(libtorrent::torrent_handle const& th);

	private:
		wxNotebook* m_notebook;
		OverviewPage* m_overview;
		FilesPage* m_files;
		PeersPage* m_peers;
		TrackersPage* m_trackers;
	};
}
