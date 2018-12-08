#include "plugin.hpp"

#include <QWidget>

class TorrentPeers : public QWidget
{
};

void TorrentDetailsPeersPlugin::load(pt::IPluginHost * const host)
{
    pt::Tab tab;
    tab.id = "torrent-details-peers";
    tab.sortOrder = 100;
    tab.title = "Peers";
    tab.widget = new TorrentPeers();

    host->torrentDetails()->addTab(tab);
}
