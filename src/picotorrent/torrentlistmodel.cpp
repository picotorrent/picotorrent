#include "torrentlistmodel.hpp"

#include <libtorrent/torrent_status.hpp>

#include "utils.hpp"

namespace lt = libtorrent;
using pt::TorrentListModel;

void TorrentListModel::addTorrent(lt::torrent_status const& status)
{
    int nextIndex = static_cast<int>(m_status.size());

    beginInsertRows(QModelIndex(), nextIndex, nextIndex);
    m_status.push_back(status);
    endInsertRows();
}

void TorrentListModel::updateTorrent(lt::torrent_status const& status)
{
    auto res = std::find_if(
        m_status.begin(),
        m_status.end(),
        [=](lt::torrent_status const& st)
        {
            return st.info_hash == status.info_hash;
        });

    if (res == m_status.end())
    {
        // Torrent not in model? How?
        throw std::runtime_error("how?");
    }

    auto idx = std::distance(m_status.begin(), res);

    m_status.at(idx) = status;

    emit dataChanged(
        index(idx, 0),
        index(idx, Columns::_Max - 1));
}

int TorrentListModel::columnCount(const QModelIndex&) const
{
    return Columns::_Max;
}

QVariant TorrentListModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    lt::torrent_status const& status = m_status.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case Columns::Name:
            return QString::fromStdString(status.name);

        case Columns::QueuePosition:
        {
            int qp = static_cast<int>(status.queue_position);

            if (qp < 0)
            {
                return "-";
            }

            return qp + 1;
        }

        case Columns::Size:
            return Utils::ToHumanFileSize(status.total_wanted);

        case Columns::Progress:
            return status.progress;
        }
    }
    case Qt::TextAlignmentRole:
    {
        switch (index.column())
        {
        case Columns::QueuePosition:
        case Columns::Size:
        {
            QFlags<Qt::AlignmentFlag> flag;
            flag |= Qt::AlignRight;
            flag |= Qt::AlignVCenter;
            return QVariant(flag);
        }
        }
        break;
    }
    }

    return QVariant();
}

QVariant TorrentListModel::headerData(int section, Qt::Orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (section)
        {
        case Columns::Name:
            return QString("Name");

        case Columns::QueuePosition:
            return QString("#");

        case Columns::Size:
            return QString("Size");

        case Columns::Progress:
            return QString("Progress");
        }

        break;
    }

    case Qt::TextAlignmentRole:
    {
        switch (section)
        {
        case Columns::QueuePosition:
        case Columns::Size:
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
    return static_cast<int>(m_status.size());
}
