#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace libtorrent
{
	struct torrent_status;
}

class wxDataViewCtrl;

namespace pt
{
    class TorrentListView : public wxPanel
    {
    public:
        TorrentListView(wxWindow* parent);

		void AddTorrent(libtorrent::torrent_status const& torrent);
		virtual wxSize GetMinSize() const;

	private:
		class Model;

		Model* m_torrentsModel;
		wxDataViewCtrl* m_torrentsView;
    };
}
