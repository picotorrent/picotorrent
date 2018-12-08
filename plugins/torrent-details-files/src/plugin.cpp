#include "plugin.hpp"

#include <QWidget>

class TorrentFiles : public QWidget
{
};

void TorrentDetailsFilesPlugin::load(pt::IPluginHost * const host)
{
    pt::Tab tab;
    tab.id = "torrent-details-files";
    tab.sortOrder = 50;
    tab.title = "Files";
    tab.widget = new TorrentFiles();

    host->torrentDetails()->addTab(tab);
}
