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
	class TrackersViewModel;

    class TrackersPage : public wxPanel
    {
	public:
		TrackersPage(wxWindow* parent, wxWindowID id);
		void Update(libtorrent::torrent_status const& ts);

	private:
		wxDataViewCtrl* m_trackersView;
		TrackersViewModel* m_viewModel;
    };
}
