#pragma once

#include <QAbstractListModel>

namespace pt
{
    class TorrentListModel : public QAbstractListModel
    {
    public:
        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex&, int role) const override;
        QModelIndex index(int, int, const QModelIndex&) const override;
        QVariant headerData(int section, Qt::Orientation, int role) const override;
        QModelIndex parent(const QModelIndex&);
        int rowCount(const QModelIndex&) const override;
    };
}
