#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class wxDataViewCtrl;

namespace libtorrent
{
	struct torrent_status;
}

namespace pt
{
	class PeersViewModel;

    class PeersPage : public wxPanel
    {
	public:
		PeersPage(wxWindow* parent, wxWindowID id);
		void Update(libtorrent::torrent_status const& ts);

	private:
		wxDataViewCtrl* m_peersView;
		PeersViewModel* m_viewModel;
    };
}
