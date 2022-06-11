#include "statusbar.hpp"

#include <fmt/format.h>
#include <boost/algorithm/string/replace.hpp>

#include "translator.hpp"
#include "../core/utils.hpp"

using pt::UI::StatusBar;

StatusBar::StatusBar(wxWindow* parent)
    : wxStatusBar(parent, wxID_ANY)
{
    int widths[] =
    {
        -1,
        -1,
        -1,
        -1
    };

    SetFieldsCount(4);
    SetStatusWidths(4, widths);
}

void StatusBar::UpdateDhtNodesCount(int64_t nodes)
{
    if (nodes < 0)
    {
        SetStatusText(i18n("dht_disabled"), 1);
    }
    else
    {
        auto wstr = i18n("dht_num_nodes");
        boost::replace_all(wstr, "{0}", "%d");
        SetStatusText(wxString::Format(_(wstr), static_cast<int>(nodes)), 1);
    }
}

void StatusBar::UpdateTorrentCount(int64_t torrents)
{
    auto wstr = i18n("num_torrents");
    boost::replace_all(wstr, "{0}", "%d");
    SetStatusText(wxString::Format(_(wstr), static_cast<int>(torrents)), 0);
}

void StatusBar::UpdateTransferRates(int64_t downSpeed, int64_t upSpeed)
{
    auto wstr = i18n("dl_s_ul_s");
    boost::replace_all(wstr, "{0}", "%s");
    boost::replace_all(wstr, "{1}", "%s");
    SetStatusText(wxString::Format(_(wstr),
                                   Utils::toHumanFileSize(downSpeed),
                                   Utils::toHumanFileSize(upSpeed)),
                  2);
}

void StatusBar::UpdateIPFilterStatus(bool enabled)
{
    if (enabled)
    {
        SetStatusText(i18n("ip_filter_enabled"), 3);
    }
    else
    {
        SetStatusText(i18n("ip_filter_disabled"), 3);
    }
}
