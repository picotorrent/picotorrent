#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class wxNotebook;

namespace pt
{
    class TorrentDetailsView : public wxPanel
    {
    public:
        TorrentDetailsView(wxWindow* parent);
		virtual wxSize GetMinSize() const;

	private:
		wxNotebook* m_notebook;
	};
}
