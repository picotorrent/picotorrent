#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt
{
    class StatusBar : public wxStatusBar
    {
    public:
        StatusBar(wxWindow* parent);

		void UpdateDhtNodesCount(int64_t nodes);
		void UpdateTorrentCount(int64_t torrents);
		void UpdateTransferRates(int64_t downSpeed, int64_t upSpeed);
    };
}
