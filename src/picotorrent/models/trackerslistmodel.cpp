#include "trackerslistmodel.hpp"

#include <libtorrent/announce_entry.hpp>

#include "../translator.hpp"

using pt::TrackersListModel;

TrackersListModel::TrackersListModel()
{
}

TrackersListModel::~TrackersListModel()
{
}

int TrackersListModel::columnCount(const QModelIndex&) const
{
    return Columns::_Max;
}

QVariant TrackersListModel::data(const QModelIndex&, int role) const
{
    return "";
}

QVariant TrackersListModel::headerData(int section, Qt::Orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (section)
        {
        case Columns::Url:
            return i18n("url");

        case Columns::Status:
            return i18n("status");

        case Columns::Fails:
            return i18n("fails");

        case Columns::NextAnnounce:
            return i18n("next_announce");
        }

        break;
    }
    }

    return QVariant();
}

QModelIndex TrackersListModel::parent(const QModelIndex&)
{
    return QModelIndex();
}

int TrackersListModel::rowCount(const QModelIndex&) const
{
    return 0;
}

