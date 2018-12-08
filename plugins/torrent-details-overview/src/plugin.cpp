#include "plugin.hpp"

#include <QWidget>

class TorrentOverview : public QWidget
{
};

void TorrentDetailsOverviewPlugin::load(pt::IPluginHost * const host)
{
    pt::Tab tab;
    tab.id = "torrent-details-overview";
    tab.sortOrder = 0;
    tab.title = "Overview";
    tab.widget = new TorrentOverview();

    host->torrentDetails()->addTab(tab);
}
