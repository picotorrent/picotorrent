#pragma once

#include <QObject>
#include <QtPlugin>

#include <picotorrent.hpp>

class TorrentDetailsPeersPlugin : public QObject, pt::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.picotorrent.IPlugin")
    Q_INTERFACES(pt::IPlugin)

public:
    void load(pt::IPluginHost * const host) override;
};
