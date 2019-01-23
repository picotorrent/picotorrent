#pragma once

#include <memory>

#include "detailstab.hpp"

namespace pt
{
    class ElidedLabel;
    struct SessionState;
    class Torrent;

    class TorrentOverviewWidget : public DetailsTab
    {
    public:
        TorrentOverviewWidget();

        virtual void clear() override;
        virtual void refresh(QList<Torrent*> const& torrents) override;

    private:
        ElidedLabel* m_name;
        ElidedLabel* m_infoHash;
        ElidedLabel* m_savePath;
        ElidedLabel* m_pieces;
    };
}
