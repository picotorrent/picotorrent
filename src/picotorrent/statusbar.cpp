#include "statusbar.hpp"

#include "utils.hpp"

using pt::StatusBar;

StatusBar::StatusBar(wxWindow* parent)
    : wxStatusBar(parent, wxID_ANY)
{
    int widths[] =
    {
        120,
        120,
        -1
    };

    SetFieldsCount(3);
    SetStatusWidths(3, widths);
}

void StatusBar::UpdateDhtNodesCount(int64_t nodes)
{
    SetStatusText(wxString::Format("DHT: %I64d node(s)", nodes), 1);
}

void StatusBar::UpdateTorrentCount(int64_t torrents)
{
    SetStatusText(wxString::Format("%I64d torrent(s)", torrents), 0);
}

void StatusBar::UpdateTransferRates(int64_t downSpeed, int64_t upSpeed)
{
    SetStatusText(
        wxString::Format("DL: %s/s, UL: %s/s",
            Utils::ToHumanFileSize(downSpeed),
            Utils::ToHumanFileSize(upSpeed)),
        2);
}
