#include "torrentlistmodel.hpp"

#include <libtorrent/torrent_status.hpp>
#include <picotorrent/core/utils.hpp>
#include <QDateTime>

#include "translator.hpp"

namespace lt = libtorrent;
using pt::TorrentListModel;

void TorrentListModel::appendInfoHashes(QModelIndexList const& indexes, std::unordered_set<lt::sha1_hash>& hashes)
{
    for (QModelIndex const& idx : indexes)
    {
        if (idx.column() > 0)
        {
            continue;
        }

        hashes.insert(m_status.at(idx.row()).info_hash);
    }
}

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
    bool paused = (status.flags & lt::torrent_flags::paused) == lt::torrent_flags::paused;

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case Columns::Name:
        {
            if (status.name.empty())
            {
                std::stringstream ss;
                ss << status.info_hash;
                return QString::fromStdString(ss.str());
            }

            return QString::fromStdString(status.name);
        }

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
            return QString::fromStdWString(Utils::toHumanFileSize(status.total_wanted));

        case Columns::Progress:
            return status.progress;

        case Columns::ETA:
        {
            if (paused)
            {
                return "-";
            }

            std::chrono::seconds secs = getEta(status);

            if (secs.count() <= 0)
            {
                return "-";
            }

            std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(secs);
            std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(secs - hours_left);
            std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(secs - hours_left - min_left);

            return QString("%1h %2m %3s").arg(
                QString::number(hours_left.count()),
                QString::number(min_left.count()),
                QString::number(sec_left.count()));
        }
        case Columns::DownloadSpeed:
        {
            if (status.download_payload_rate > 0 && !paused)
            {
                return QString("%1/s").arg(Utils::toHumanFileSize(status.download_payload_rate));
            }

            return "-";
        }
        case Columns::UploadSpeed:
        {
            if (status.upload_payload_rate > 0 && !paused)
            {
                return QString("%1/s").arg(Utils::toHumanFileSize(status.upload_payload_rate));
            }

            return "-";
        }
        case Columns::Availability:
        {
            if (status.distributed_copies >= 0 && !paused)
            {
                QString str;
                str.sprintf("%.3f", status.distributed_copies);
                return str;
            }

            return "-";
        }
        case Columns::Ratio:
        {
            float ratio = getRatio(status);
            QString str;
            str.sprintf("%.3f", ratio);
            return str;
        }
        case Columns::Seeds:
        {
            if (paused)
            {
                return "-";
            }

            QString str;
            str.sprintf(
                i18n("d_of_d").toLocal8Bit().data(),
                status.num_seeds,
                status.list_seeds);
            return str;
        }
        case Columns::Peers:
        {
            if (paused)
            {
                return "-";
            }

            QString str;
            str.sprintf(
                i18n("d_of_d").toLocal8Bit().data(),
                status.num_peers - status.num_seeds,
                status.list_peers - status.list_seeds);
            return str;
        }
        case Columns::AddedOn:
        {
            return QDateTime::fromSecsSinceEpoch(status.added_time);
        }
        case Columns::CompletedOn:
        {
            if (status.completed_time > 0)
            {
                return QDateTime::fromSecsSinceEpoch(status.completed_time);
            }

            return "-";
        }
        }
    }
    case Qt::TextAlignmentRole:
    {
        switch (index.column())
        {
        case Columns::QueuePosition:
        case Columns::Size:
        case Columns::ETA:
        case Columns::DownloadSpeed:
        case Columns::UploadSpeed:
        case Columns::Availability:
        case Columns::Ratio:
        case Columns::Seeds:
        case Columns::Peers:
        case Columns::AddedOn:
        case Columns::CompletedOn:
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
            return i18n("name");

        case Columns::QueuePosition:
            return i18n("queue_position");

        case Columns::Size:
            return i18n("size");

        case Columns::Progress:
            return i18n("progress");

        case Columns::ETA:
            return i18n("eta");

        case Columns::DownloadSpeed:
            return i18n("dl");

        case Columns::UploadSpeed:
            return i18n("ul");

        case Columns::Availability:
            return i18n("availability");

        case Columns::Ratio:
            return i18n("ratio");

        case Columns::Seeds:
            return i18n("seeds");

        case Columns::Peers:
            return i18n("peers");

        case Columns::AddedOn:
            return i18n("added_on");

        case Columns::CompletedOn:
            return i18n("completed_on");
        }

        break;
    }

    case Qt::TextAlignmentRole:
    {
        switch (section)
        {
        case Columns::QueuePosition:
        case Columns::Size:
        case Columns::ETA:
        case Columns::DownloadSpeed:
        case Columns::UploadSpeed:
        case Columns::Availability:
        case Columns::Ratio:
        case Columns::Seeds:
        case Columns::Peers:
        case Columns::AddedOn:
        case Columns::CompletedOn:
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


std::chrono::seconds TorrentListModel::getEta(const lt::torrent_status& ts) const
{
    int64_t remaining_bytes = ts.total_wanted - ts.total_wanted_done;

    if (remaining_bytes > 0 && ts.download_payload_rate > 0)
    {
        return std::chrono::seconds(remaining_bytes / ts.download_payload_rate);
    }

    return std::chrono::seconds(0);
}

float TorrentListModel::getRatio(const lt::torrent_status& ts) const
{
    if (ts.all_time_download > 0)
    {
        return static_cast<float>(ts.all_time_upload) / static_cast<float>(ts.all_time_download);
    }

    return 0;
}

