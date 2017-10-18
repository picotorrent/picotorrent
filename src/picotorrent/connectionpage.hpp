#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
	class Translator;

    class ConnectionPage : public wxPanel
    {
    public:
        ConnectionPage(wxWindow* parent, std::shared_ptr<Translator> translator);

	private:
		wxTextCtrl* m_listenInterfaces;
	};
}
