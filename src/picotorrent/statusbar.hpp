#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    class Translator;

    class StatusBar : public wxStatusBar
    {
    public:
        StatusBar(wxWindow* parent, std::shared_ptr<Translator> translator);

        void UpdateDhtNodesCount(int64_t nodes);
        void UpdateTorrentCount(int64_t torrents);
        void UpdateTransferRates(int64_t downSpeed, int64_t upSpeed);

    private:
        std::shared_ptr<Translator> m_translator;
    };
}
