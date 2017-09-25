#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace libtorrent
{
	class session;
}

class wxNotebook;
class wxSplitterWindow;

namespace pt
{
	class TorrentDetailsView;
	class TorrentListView;

    class MainFrame : public wxFrame
    {
    public:
		MainFrame();
		~MainFrame();

    private:
		enum
		{
			ptID_ADD_TORRENTS = wxID_HIGHEST + 1
		};

		void OnAbout(wxCommandEvent&);
		void OnAddTorrents(wxCommandEvent&);
		void OnExit(wxCommandEvent&);

        wxDECLARE_EVENT_TABLE();

		wxSplitterWindow* m_splitter;
		TorrentListView* m_torrentListView;
		TorrentDetailsView* m_torrentDetailsView;

		std::shared_ptr<libtorrent::session> m_session;
    };
}
