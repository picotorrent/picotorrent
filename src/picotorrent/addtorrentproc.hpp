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

    class AddTorrentProcedure
    {
    public:
        AddTorrentProcedure(wxWindow* parent, std::shared_ptr<Translator> translator, std::shared_ptr<SessionState> state);
        void Execute();

	private:
		wxWindow* m_parent;
		std::shared_ptr<SessionState> m_state;
		std::shared_ptr<Translator> m_trans;
    };
}
