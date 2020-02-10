#pragma once

#include <QAbstractListModel>

#include <chrono>
#include <string>
#include <vector>

#include <libtorrent/fwd.hpp>

namespace pt
{
    class TorrentHandle;

    class TrackersListModel : public QAbstractListModel
    {
    public:
        enum Columns
        {
            Url,
            Status,
            NumDownloaded,
            NumLeeches,
            NumSeeds,
            Fails,
            NextAnnounce,
            _Max
        };

        enum ListItemStatus
        {
            unknown,
            error,
            working,
            updating,
        };

        struct ListItem
        {
            std::string key;
            bool isTier;
            int tier;
            ListItemStatus status;
            int numDownloaded;
            int numLeeches;
            int numSeeds;
            std::chrono::seconds nextAnnounce;
            std::string errorMessage;
            int fails;
            int failLimit;
        };

        TrackersListModel();
        virtual ~TrackersListModel();

        void clear();
        void update(TorrentHandle* torrent);

        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex&, int role) const override;
        QVariant headerData(int section, Qt::Orientation, int role) const override;
        QModelIndex parent(const QModelIndex&);
        int rowCount(const QModelIndex&) const override;

    private:
        ListItem* m_dhtTrackerStatus;
        ListItem* m_lsdTrackerStatus;
        ListItem* m_pexTrackerStatus;

        std::vector<ListItem> m_trackers;
    };
}

Q_DECLARE_METATYPE(pt::TrackersListModel::ListItem)
