#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
	struct SessionState;

    class MainMenu : public wxMenuBar
    {
    public:
        MainMenu(std::shared_ptr<SessionState> state);

	private:
		enum
		{
			ptID_ADD_TORRENTS = wxID_HIGHEST + 1
		};

		wxDECLARE_EVENT_TABLE();

		void OnAbout(wxCommandEvent&);
		void OnAddTorrents(wxCommandEvent&);
		void OnExit(wxCommandEvent&);

		std::shared_ptr<SessionState> m_state;
    };
}
