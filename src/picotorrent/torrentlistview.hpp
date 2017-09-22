#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt
{
    class TorrentListView : public wxPanel
    {
    public:
        TorrentListView(wxWindow* parent);
		virtual wxSize GetMinSize() const;
    };
}
