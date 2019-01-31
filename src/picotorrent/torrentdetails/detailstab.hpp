#pragma once

#include <QList>
#include <QWidget>

namespace pt
{
    class TorrentHandle;

    struct DetailsTab : public QWidget
    {
        virtual void clear() = 0;
        virtual void refresh(QList<TorrentHandle*> const& torrents) = 0;
    };
}
