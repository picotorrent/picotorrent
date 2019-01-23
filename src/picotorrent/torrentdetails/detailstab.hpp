#pragma once

#include <QList>
#include <QWidget>

namespace pt
{
    class Torrent;

    struct DetailsTab : public QWidget
    {
        virtual void clear() = 0;
        virtual void refresh(QList<Torrent*> const& torrents) = 0;
    };
}
