#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
	struct SessionState;
	class Translator;

    class MainMenu : public wxMenuBar
    {
    public:
        MainMenu(std::shared_ptr<SessionState> state, std::shared_ptr<Translator> translator);

	private:
		enum
		{
			ptID_ADD_TORRENTS = wxID_HIGHEST + 1,
			ptID_VIEW_PREFERENCES
		};

		wxDECLARE_EVENT_TABLE();

		void OnAbout(wxCommandEvent&);
		void OnAddTorrents(wxCommandEvent&);
		void OnExit(wxCommandEvent&);
		void OnViewPreferences(wxCommandEvent&);

		std::shared_ptr<SessionState> m_state;
		std::shared_ptr<Translator> m_trans;
    };
}
