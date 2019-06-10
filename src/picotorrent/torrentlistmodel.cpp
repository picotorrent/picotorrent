#include "torrentlistmodel.hpp"

#include <libtorrent/torrent_status.hpp>

#include <QDateTime>

#include "core/utils.hpp"
#include "torrenthandle.hpp"
#include "torrentstatus.hpp"
#include "translator.hpp"

namespace lt = libtorrent;
using pt::TorrentListModel;

TorrentListModel::TorrentListModel()
{
}

TorrentListModel::~TorrentListModel()
{
}

void TorrentListModel::addTorrent(pt::TorrentHandle* torrent)
{
    int nextIndex = static_cast<int>(m_torrents.size());

    beginInsertRows(QModelIndex(), nextIndex, nextIndex);
    m_torrents.push_back(torrent);
    endInsertRows();
}

void TorrentListModel::removeTorrent(pt::TorrentHandle* torrent)
{
    auto res = std::find_if(
        m_torrents.begin(),
        m_torrents.end(),
        [=](TorrentHandle* t)
        {
            return t == torrent;
        });

    if (res == m_torrents.end())
    {
        // Torrent not in model? How?
        throw std::runtime_error("how?");
    }

    auto idx = std::distance(m_torrents.begin(), res);

    beginRemoveRows(QModelIndex(), idx, idx);
    m_torrents.erase(res);
    endRemoveRows();
}

void TorrentListModel::updateTorrent(pt::TorrentHandle* torrent)
{
    auto res = std::find_if(
        m_torrents.begin(),
        m_torrents.end(),
        [=](TorrentHandle* t)
        {
            return t == torrent;
        });

    if (res == m_torrents.end())
    {
        // Torrent not in model? How?
        throw std::runtime_error("how?");
    }

    auto idx = std::distance(m_torrents.begin(), res);

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

    TorrentHandle* torrent = m_torrents.at(index.row());
    TorrentStatus  status  = torrent->status();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case Columns::Name:
            return status.name;

        case Columns::QueuePosition:
            return status.queuePosition < 0
                ? QVariant("-")
                : QVariant(status.queuePosition + 1);

        case Columns::Size:
            return QString::fromStdWString(Utils::toHumanFileSize(status.totalWanted));

        case Columns::SizeRemaining:
            return status.totalWantedRemaining <= 0
                ? "-"
                : QString::fromStdWString(Utils::toHumanFileSize(status.totalWantedRemaining));

        case Columns::Status:
        {
            switch (status.state)
            {
            case TorrentStatus::State::CheckingFiles:
            case TorrentStatus::State::DownloadingChecking:
                return i18n("state_downloading_checking");

            case TorrentStatus::State::CheckingResumeData:
                return i18n("state_checking_resume_data");

            case TorrentStatus::State::Downloading:
                return i18n("state_downloading");

            case TorrentStatus::State::DownloadingMetadata:
                return i18n("state_downloading_metadata");

            case TorrentStatus::State::DownloadingPaused:
                return i18n("state_downloading_paused");

            case TorrentStatus::State::DownloadingQueued:
                return i18n("state_downloading_queued");

            case TorrentStatus::State::Error:
                return i18n("state_error").arg(status.error.toLocal8Bit().data());

            case TorrentStatus::State::Unknown:
                return i18n("state_unknown");

            case TorrentStatus::State::Uploading:
                return i18n("state_uploading");

            case TorrentStatus::State::UploadingPaused:
                return i18n("state_uploading_paused");

            case TorrentStatus::State::UploadingQueued:
                return i18n("state_uploading_queued");
            }

            return "-";
        }

        case Columns::Progress:
            return status.progress;

        case Columns::ETA:
        {
            // TODO: check if paused

            if (status.eta.count() <= 0)
            {
                return "-";
            }

            std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(status.eta);
            std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(status.eta - hours_left);
            std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(status.eta - hours_left - min_left);

            return QString("%1h %2m %3s").arg(
                QString::number(hours_left.count()),
                QString::number(min_left.count()),
                QString::number(sec_left.count()));
        }

        case Columns::DownloadSpeed:
            // TODO: check if paused
            return status.downloadPayloadRate > 0
                ? QString("%1/s").arg(Utils::toHumanFileSize(status.downloadPayloadRate))
                : "-";

        case Columns::UploadSpeed:
            // TODO: check if paused
            return status.uploadPayloadRate > 0
                ? QString("%1/s").arg(Utils::toHumanFileSize(status.uploadPayloadRate))
                : "-";

        case Columns::Availability:
            // TODO: check if paused
            return status.availability >= 0
                ? QString::asprintf("%.3f", status.availability)
                : "-";

        case Columns::Ratio:
            return QString::asprintf("%.3f", status.ratio);

        case Columns::Seeds:
            // TODO: check if paused
            return QString::asprintf(
                i18n("d_of_d").toLocal8Bit().data(),
                status.seedsCurrent,
                status.seedsTotal);

        case Columns::Peers:
            // TODO: check if paused
            return QString::asprintf(
                i18n("d_of_d").toLocal8Bit().data(),
                status.peersCurrent,
                status.peersTotal);

        case Columns::AddedOn:
            return status.addedOn.toString("yyyy-MM-dd HH:mm:ss");

        case Columns::CompletedOn:
            return status.completedOn.isNull()
                ? "-"
                : status.completedOn.toString("yyyy-MM-dd HH:mm:ss");
        }
    }
    case Qt::TextAlignmentRole:
    {
        switch (index.column())
        {
        case Columns::QueuePosition:
        case Columns::Size:
        case Columns::SizeRemaining:
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
    case Qt::UserRole:
        return QVariant::fromValue(static_cast<void*>(torrent));
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

        case Columns::SizeRemaining:
            return i18n("size_remaining");

        case Columns::Status:
            return i18n("status");

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
        case Columns::SizeRemaining:
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
    return static_cast<int>(m_torrents.size());
}
