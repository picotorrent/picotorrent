#pragma once

#include <QTreeView>

#include <picotorrent.hpp>

namespace pt
{
    class TorrentListWidget : public QTreeView, public ITorrentListWidget
    {
    public:
        TorrentListWidget();
    };
}
