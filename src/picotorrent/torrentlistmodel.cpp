#include "torrentlistmodel.hpp"

using pt::TorrentListModel;

int TorrentListModel::columnCount(const QModelIndex&) const
{
    return 3;
}

QVariant TorrentListModel::data(const QModelIndex&, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return QString("Hello");
    }

    return QVariant();
}

QModelIndex TorrentListModel::index(int, int, const QModelIndex&) const
{
    return QModelIndex();
}

QVariant TorrentListModel::headerData(int section, Qt::Orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (section)
        {
        case 0:
            return QString("Name");

        case 1:
            return QString("#");

        case 2:
            return QString("Size");
        }

        break;
    }

    case Qt::TextAlignmentRole:
    {
        switch (section)
        {
        case 1:
            return Qt::AlignRight;
        }
        break;
    }
    }

    return QVariant();
}

QModelIndex TorrentListModel::parent(const QModelIndex&)
{
    return QModelIndex();
}

int TorrentListModel::rowCount(const QModelIndex&) const
{
    return 1;
}
