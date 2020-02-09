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
            Fails,
            NextAnnounce,
            _Max
        };

        TrackersListModel();
        virtual ~TrackersListModel();

        void clear();
        void update(TorrentHandle* torrent);

        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex&, int role) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const;
        QVariant headerData(int section, Qt::Orientation, int role) const override;
        QModelIndex parent(const QModelIndex&);
        int rowCount(const QModelIndex&) const override;

    private:
        enum ListItemStatus
        {
            Error,
            Working,
            Updating,
        };

        struct ListItem
        {
            std::string key;
            bool isTier;
            std::uint8_t tier;
            ListItemStatus status;
            int peers;
            int seeds;
            std::chrono::seconds nextAnnounce;
        };

        std::vector<ListItem> m_trackers;
    };
}
