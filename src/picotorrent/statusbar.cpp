#include "statusbar.hpp"

#include "translator.hpp"
#include "utils.hpp"

using pt::StatusBar;

StatusBar::StatusBar(wxWindow* parent, std::shared_ptr<pt::Translator> translator)
    : wxStatusBar(parent, wxID_ANY),
    m_translator(translator)
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
    if (nodes < 0)
    {
        SetStatusText(i18n(m_translator, "dht_disabled"), 1);
    }
    else
    {
        SetStatusText(wxString::Format(i18n(m_translator, "dht_i64d_nodes"), nodes), 1);
    }
}

void StatusBar::UpdateTorrentCount(int64_t torrents)
{
    SetStatusText(wxString::Format(i18n(m_translator, "i64d_torrents"), torrents), 0);
}

void StatusBar::UpdateTransferRates(int64_t downSpeed, int64_t upSpeed)
{
    SetStatusText(
        wxString::Format(i18n(m_translator, "dl_s_ul_s"),
            Utils::ToHumanFileSize(downSpeed),
            Utils::ToHumanFileSize(upSpeed)),
        2);
}
