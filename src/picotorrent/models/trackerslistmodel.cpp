#include "trackerslistmodel.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/torrent_handle.hpp>

#include "../torrenthandle.hpp"
#include "../translator.hpp"

using pt::TrackersListModel;

TrackersListModel::TrackersListModel()
{
}

TrackersListModel::~TrackersListModel()
{
}

void TrackersListModel::clear()
{
    this->beginResetModel();
    m_trackers.clear();
    this->endResetModel();
}

void TrackersListModel::update(pt::TorrentHandle* torrent)
{
    std::vector<lt::announce_entry> trackers = torrent->trackers();

    // Remove old data
    for (auto it = m_trackers.begin(); it != m_trackers.end();)
    {
        auto f = std::find_if(trackers.begin(), trackers.end(), [it](lt::announce_entry& ae) { return ae.url == it->url; });

        if (f == trackers.end())
        {
            auto distance = std::distance(m_trackers.begin(), it);

            this->beginRemoveRows(QModelIndex(), distance, distance);
            it = m_trackers.erase(it);
            this->endRemoveRows();
        }
        else
        {
            ++it;
        }
    }

    // Add or update new data
    for (auto it = trackers.begin(); it != trackers.end(); it++)
    {
        auto f = std::find_if(m_trackers.begin(), m_trackers.end(), [it](lt::announce_entry& ae) { return ae.url == it->url; });

        if (f == m_trackers.end())
        {
            int nextIndex = static_cast<int>(m_trackers.size());

            this->beginInsertRows(QModelIndex(), nextIndex, nextIndex);
            m_trackers.push_back(*it);
            this->endInsertRows();
        }
        else
        {
            auto distance = std::distance(m_trackers.begin(), f);
            m_trackers.at(distance) = *it;

            emit dataChanged(
                index(distance, 0),
                index(distance, Columns::_Max - 1));
        }
    }
}

int TrackersListModel::columnCount(const QModelIndex&) const
{
    return Columns::_Max;
}

QVariant TrackersListModel::data(const QModelIndex& index, int role) const
{
    lt::announce_entry const& entry = m_trackers.at(index.row());

    auto endp = std::min_element(
        entry.endpoints.begin(),
        entry.endpoints.end(),
        [](lt::announce_endpoint const& l, lt::announce_endpoint const& r)
		{
			return l.info_hashes[lt::protocol_version::V1].fails < r.info_hashes[lt::protocol_version::V1].fails;
		});

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case Columns::Url:
            return QString::fromStdString(entry.url);

        case Columns::Status:
        {
            if (endp == entry.endpoints.end())
            {
                return "-";
            }

			auto announce_hash = endp->info_hashes[lt::protocol_version::V1];

            if (announce_hash.updating)
            {
                return i18n("updating");
            }

            if (announce_hash.last_error)
            {
                return i18n("error_s").arg(
                    announce_hash.message.empty()
                        ? announce_hash.last_error.message().c_str()
                        : QString("%1 \"%2\"")
                            .arg(QString::fromStdString(announce_hash.last_error.message()))
                            .arg(QString::fromStdString(announce_hash.message)));
            }

            if (entry.verified)
            {
                return i18n("ok");
            }

            return "-";
        }

        case Columns::Fails:
        {
            if (endp != entry.endpoints.end()
				&& endp->info_hashes[lt::protocol_version::V1].fails == 0)
            {
                return "-";
            }

            if (entry.fail_limit == 0)
            {
                return QString::number(
					endp != entry.endpoints.end()
						? endp->info_hashes[lt::protocol_version::V1].fails
						: 0);
            }

            return i18n("d_of_d")
                .arg(endp != entry.endpoints.end()
                    ? endp->info_hashes[lt::protocol_version::V1].fails
                    : 0)
                .arg(entry.fail_limit);
        }

        case Columns::NextAnnounce:
        {
            if (endp == entry.endpoints.end()
                || endp->info_hashes[lt::protocol_version::V1].updating)
            {
                return "-";
            }

            int64_t secs = lt::total_seconds(endp->info_hashes[lt::protocol_version::V1].next_announce - lt::clock_type::now());
            std::chrono::seconds s(secs);

            if (secs <= 0)
            {
                return "-";
            }

            std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(s);
            std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(s - hours_left);
            std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(s - hours_left - min_left);

            return QString("%1h %2m %3s").arg(
                hours_left.count(),
                min_left.count(),
                sec_left.count());
        }
        }

        break;
    }
    }

    return QVariant();
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
    return static_cast<int>(m_trackers.size());
}
