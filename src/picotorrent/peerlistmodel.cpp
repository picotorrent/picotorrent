#include "peerlistmodel.hpp"

#include <libtorrent/peer_info.hpp>

#include "translator.hpp"

using pt::PeerListModel;

PeerListModel::PeerListModel()
{
}

PeerListModel::~PeerListModel()
{
}

int PeerListModel::columnCount(const QModelIndex&) const
{
    return Columns::_Max;
}

QVariant PeerListModel::data(const QModelIndex&, int role) const
{
    return "";
}

QVariant PeerListModel::headerData(int section, Qt::Orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (section)
        {
        case Columns::IP:
            return i18n("ip");

        case Columns::Client:
            return i18n("client");

        case Columns::Flags:
            return i18n("flags");

        case Columns::DownloadRate:
            return i18n("dl");

        case Columns::UploadRate:
            return i18n("ul");

        case Columns::Progress:
            return i18n("progress");
        }

        break;
    }
    }

    return QVariant();
}

QModelIndex PeerListModel::parent(const QModelIndex&)
{
    return QModelIndex();
}

int PeerListModel::rowCount(const QModelIndex&) const
{
    return 0;
}
