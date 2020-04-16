#include "statusbar.hpp"

#include "translator.hpp"
#include "../core/utils.hpp"

using pt::UI::StatusBar;

StatusBar::StatusBar(wxWindow* parent)
    : wxStatusBar(parent, wxID_ANY)
{
    int widths[] =
    {
        FromDIP(120),
        FromDIP(120),
        -1
    };

    SetFieldsCount(3);
    SetStatusWidths(3, widths);
}

void StatusBar::UpdateDhtNodesCount(int64_t nodes)
{
    if (nodes < 0)
    {
        SetStatusText(i18n("dht_disabled"), 1);
    }
    else
    {
        SetStatusText(wxString::Format(wxString::FromUTF8(i18n("dht_i64d_nodes")), nodes), 1);
    }
}

void StatusBar::UpdateTorrentCount(int64_t torrents)
{
    SetStatusText(wxString::Format(wxString::FromUTF8(i18n("i64d_torrents")), torrents), 0);
}

void StatusBar::UpdateTransferRates(int64_t downSpeed, int64_t upSpeed)
{
    SetStatusText(
        wxString::Format(
            wxString::FromUTF8(i18n("dl_s_ul_s")),
            Utils::toHumanFileSize(downSpeed),
            Utils::toHumanFileSize(upSpeed)),
        2);
}
