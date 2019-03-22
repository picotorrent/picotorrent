#pragma once

#include <QSortFilterProxyModel>

namespace pt
{
    class TorrentSortFilterProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT

    public:
        TorrentSortFilterProxyModel(QObject* parent = nullptr);

    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    };
}
