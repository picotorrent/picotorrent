#include "torrentsortfilterproxymodel.hpp"

#include "torrenthandle.hpp"
#include "torrentlistmodel.hpp"
#include "torrentstatus.hpp"

using pt::TorrentSortFilterProxyModel;

TorrentSortFilterProxyModel::TorrentSortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

bool TorrentSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    return true;
}

bool TorrentSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left, Qt::UserRole);
    QVariant rightData = sourceModel()->data(right, Qt::UserRole);

    auto leftTorrent = static_cast<TorrentHandle*>(leftData.value<void*>());
    auto rightTorrent = static_cast<TorrentHandle*>(rightData.value<void*>());

    auto leftStatus = leftTorrent->status();
    auto rightStatus = rightTorrent->status();

    switch (left.column())
    {
    case TorrentListModel::Columns::Name:
        return QString::localeAwareCompare(
            leftStatus.name,
            rightStatus.name) < 0;
    case TorrentListModel::Columns::QueuePosition:
        return leftStatus.queuePosition < rightStatus.queuePosition;
    case TorrentListModel::Columns::Size:
        return leftStatus.totalWanted < rightStatus.totalWanted;
    case TorrentListModel::Columns::Status:
        return leftStatus.state < rightStatus.state;
    case TorrentListModel::Columns::Progress:
        return leftStatus.progress < rightStatus.progress;
    case TorrentListModel::Columns::ETA:
        return leftStatus.eta.count() < rightStatus.eta.count();
    case TorrentListModel::Columns::DownloadSpeed:
        return leftStatus.downloadPayloadRate < rightStatus.downloadPayloadRate;
    case TorrentListModel::Columns::UploadSpeed:
        return leftStatus.uploadPayloadRate < rightStatus.uploadPayloadRate;
    case TorrentListModel::Columns::Availability:
        return leftStatus.availability < rightStatus.availability;
    case TorrentListModel::Columns::Ratio:
        return leftStatus.ratio < rightStatus.ratio;
    case TorrentListModel::Columns::Seeds:
        return leftStatus.seedsCurrent < rightStatus.seedsCurrent;
    case TorrentListModel::Columns::Peers:
        return leftStatus.peersCurrent < rightStatus.peersCurrent;
    case TorrentListModel::Columns::AddedOn:
        return leftStatus.addedOn < rightStatus.addedOn;
    case TorrentListModel::Columns::CompletedOn:
        return leftStatus.completedOn < rightStatus.completedOn;
    default:
        return false;
    }
}
