#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxSplitterWindow;

namespace pt
{
	class Environment;
	struct SessionState;
	class TorrentDetailsView;
	class TorrentListView;

    class MainFrame : public wxFrame
    {
    public:
		MainFrame(std::shared_ptr<Environment> env);
		~MainFrame();

    private:
		enum
		{
			ptID_ADD_TORRENTS = wxID_HIGHEST + 1,
			ptID_MAIN_TIMER
		};

		void OnAbout(wxCommandEvent&);
		void OnAddTorrents(wxCommandEvent&);
		void OnExit(wxCommandEvent&);
		void OnSessionAlert();
		void OnTimer(wxTimerEvent&);

        wxDECLARE_EVENT_TABLE();

		wxSplitterWindow* m_splitter;
		wxTimer* m_timer;
		TorrentListView* m_torrentListView;
		TorrentDetailsView* m_torrentDetailsView;

		std::shared_ptr<Environment> m_env;
		std::shared_ptr<SessionState> m_state;
    };
}
