#include "trackerslistmodel.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/torrent_handle.hpp>

#include <QColor>
#include <QDebug>
#include <QFont>

#include "../torrenthandle.hpp"
#include "../translator.hpp"

using pt::TrackersListModel;

TrackersListModel::TrackersListModel()
    : m_dhtTrackerStatus(nullptr),
    m_lsdTrackerStatus(nullptr),
    m_pexTrackerStatus(nullptr)
{
}

TrackersListModel::~TrackersListModel()
{
}

void TrackersListModel::clear()
{
    this->beginResetModel();

    m_dhtTrackerStatus = nullptr;
    m_lsdTrackerStatus = nullptr;
    m_pexTrackerStatus = nullptr;

    m_trackers.clear();

    this->endResetModel();
}

void TrackersListModel::update(pt::TorrentHandle* torrent)
{
    // Insert static items
    if (m_dhtTrackerStatus == nullptr)
    {
        ListItem dht;
        dht.isTier = false;
        dht.key = "DHT";
        dht.numDownloaded = -1;
        dht.numLeeches = 0;
        dht.numSeeds = 0;
        dht.tier = -1;

        m_trackers.insert(m_trackers.begin(), dht);

        m_dhtTrackerStatus = &m_trackers[0];
    }

    if (m_lsdTrackerStatus == nullptr)
    {
        ListItem lsd;
        lsd.isTier = false;
        lsd.key = "LSD";
        lsd.numDownloaded = -1;
        lsd.numLeeches = 0;
        lsd.numSeeds = 0;
        lsd.tier = -1;

        m_trackers.insert(m_trackers.begin() + 1, lsd);

        m_lsdTrackerStatus = &m_trackers[1];
    }

    if (m_pexTrackerStatus == nullptr)
    {
        ListItem pex;
        pex.isTier = false;
        pex.key = "PeX";
        pex.numDownloaded = -1;
        pex.numLeeches = 0;
        pex.numSeeds = 0;
        pex.tier = -1;

        m_trackers.insert(m_trackers.begin() + 2, pex);

        m_pexTrackerStatus = &m_trackers[2];
    }

    std::vector<lt::announce_entry> trackers = torrent->trackers();

    std::vector<lt::peer_info> peers;
    torrent->getPeerInfo(peers);

    m_dhtTrackerStatus->numLeeches = m_dhtTrackerStatus->numSeeds = 0;
    m_lsdTrackerStatus->numLeeches = m_lsdTrackerStatus->numSeeds = 0;
    m_pexTrackerStatus->numLeeches = m_pexTrackerStatus->numSeeds = 0;

    for (lt::peer_info const& peer : peers)
    {
        bool seed = static_cast<bool>(peer.flags & lt::peer_info::seed);

        if (static_cast<bool>(peer.source & lt::peer_info::dht))
        {
            if (seed) m_dhtTrackerStatus->numSeeds += 1;
            else      m_dhtTrackerStatus->numLeeches += 1;
        }
        else if (static_cast<bool>(peer.source & lt::peer_info::lsd))
        {
            if (seed) m_lsdTrackerStatus->numSeeds += 1;
            else      m_lsdTrackerStatus->numLeeches += 1;
        }
        else if (static_cast<bool>(peer.source & lt::peer_info::pex))
        {
            if (seed) m_pexTrackerStatus->numSeeds += 1;
            else      m_pexTrackerStatus->numLeeches += 1;
        }
    }

    for (auto it = m_trackers.begin(); it != m_trackers.end();)
    {
        // if it's a tier or if the tier is < 0 meaning it's a static item
        if (it->isTier || it->tier < 0)
        {
            ++it;
            continue;
        }

        auto f = std::find_if(
            trackers.begin(),
            trackers.end(),
            [it](lt::announce_entry& ae)
            {
                return ae.url == it->key
                    && ae.tier == it->tier;
            });

        if (f == trackers.end())
        {

            // If we only have one two items in this tier, one must be the tier label
            // and if so, remove both.

            int tierCount = std::count_if(
                m_trackers.begin(),
                m_trackers.end(),
                [it](ListItem const& li)
                {
                    return li.tier == it->tier;
                });

            if (tierCount == 2)
            {
                --it;

                auto distance = std::distance(m_trackers.begin(), it);

                this->beginRemoveRows(QModelIndex(), distance, distance + 1);
                it = m_trackers.erase(it);
                it = m_trackers.erase(it);
                this->endRemoveRows();
            }
            else
            {
                auto distance = std::distance(m_trackers.begin(), it);

                this->beginRemoveRows(QModelIndex(), distance, distance);
                it = m_trackers.erase(it);
                this->endRemoveRows();
            }
        }
        else
        {
            ++it;
        }
    }

    // Add or update new data
    for (auto it = trackers.begin(); it != trackers.end(); it++)
    {
        auto endpoint = std::min_element(
            it->endpoints.begin(),
            it->endpoints.end(),
            [](lt::announce_endpoint const& l, lt::announce_endpoint const& r)
            {
                return l.info_hashes[lt::protocol_version::V1].fails < r.info_hashes[lt::protocol_version::V1].fails;
            });

        ListItem trackerItem;
        trackerItem.isTier = false;
        trackerItem.key = it->url;
        trackerItem.numDownloaded = -1;
        trackerItem.numLeeches = -1;
        trackerItem.numSeeds = -1;
        trackerItem.status = ListItemStatus::unknown;
        trackerItem.tier = it->tier;

        for (lt::announce_endpoint const& ep : it->endpoints)
        {
            lt::announce_infohash const& ah = ep.info_hashes[lt::protocol_version::V1];

            trackerItem.numDownloaded = std::max(trackerItem.numDownloaded, ah.scrape_downloaded);
            trackerItem.numLeeches = std::max(trackerItem.numLeeches, ah.scrape_incomplete);
            trackerItem.numSeeds = std::max(trackerItem.numSeeds, ah.scrape_complete);
        }

        if (endpoint != it->endpoints.end())
        {
            lt::announce_infohash const& ah = endpoint->info_hashes[lt::protocol_version::V1];

            trackerItem.failLimit = it->fail_limit;
            trackerItem.fails = ah.fails;
            trackerItem.nextAnnounce = std::chrono::seconds(lt::total_seconds(ah.next_announce - lt::clock_type::now()));

            if (ah.updating)
            {
                trackerItem.status = ListItemStatus::updating;
            }
            else if (ah.last_error)
            {
                QString error = i18n("error_s")
                    .arg(ah.message.empty()
                            ? ah.last_error.message().c_str()
                            : QString("%1 \"%2\"")
                    .arg(QString::fromStdString(ah.last_error.message()))
                    .arg(QString::fromStdString(ah.message)));

                trackerItem.errorMessage = error.toStdString();
                trackerItem.status = ListItemStatus::error;
            }
            else if (it->verified)
            {
                trackerItem.status = ListItemStatus::working;
            }
        }

        auto f = std::find_if(
            m_trackers.begin(),
            m_trackers.end(),
            [it](ListItem& item)
            {
                return item.key == it->url && item.tier == it->tier;
            });

        if (f == m_trackers.end())
        {
            // Find end of tier
            auto endOfTier = std::find_if(
                m_trackers.rbegin(),
                m_trackers.rend(),
                [it](ListItem& item)
                {
                    return item.tier == it->tier && !item.isTier;
                });

            int insertIndex = endOfTier == m_trackers.rend()
                ? static_cast<int>(m_trackers.size())
                : std::distance(m_trackers.rbegin(), endOfTier);

            if (endOfTier == m_trackers.rend())
            {
                // Insert tier and then insert item at end
                ListItem tierItem;
                tierItem.isTier = true;
                tierItem.tier = it->tier;

                this->beginInsertRows(QModelIndex(), insertIndex, insertIndex + 1);
                m_trackers.push_back(tierItem);
                m_trackers.push_back(trackerItem);
                this->endInsertRows();
            }
            else
            {
                this->beginInsertRows(QModelIndex(), insertIndex, insertIndex);
                m_trackers.insert(endOfTier.base(), trackerItem);
                this->endInsertRows();
            }
        }
        else
        {
            auto distance = std::distance(m_trackers.begin(), f);
            m_trackers.at(distance) = trackerItem;

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
    ListItem const& item = m_trackers.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case Columns::Url:
        {
            if (item.isTier)
            {
                return i18n("tier_n")
                    .arg(item.tier);
            }

            return QString::fromStdString(item.key);
        }

        case Columns::Status:
        {
            if (item.isTier)
            {
                return "";
            }

            switch (item.status)
            {
            case ListItemStatus::updating:
                return i18n("updating");
            case ListItemStatus::error:
                return QString::fromStdString(item.errorMessage);
            case ListItemStatus::working:
                return i18n("ok");
            default:
                return "-";
            }
        }

        case Columns::NumDownloaded:
        {
            if (item.isTier ||item.tier < 0)
            {
                return "";
            }

            return item.numDownloaded >= 0
                ? QString::number(item.numDownloaded)
                : i18n("not_available");
        }

        case Columns::NumLeeches:
        {
            if (item.isTier)
            {
                return "";
            }

            return item.numLeeches >= 0
                ? QString::number(item.numLeeches)
                : i18n("not_available");
        }

        case Columns::NumSeeds:
        {
            if (item.isTier)
            {
                return "";
            }

            return item.numSeeds >= 0
                ? QString::number(item.numSeeds)
                : i18n("not_available");
        }

        case Columns::Fails:
        {
            if (item.isTier || item.tier < 0)
            {
                return "";
            }

            if (item.fails == 0)
            {
                return "-";
            }

            if (item.failLimit == 0)
            {
                return QString::number(item.fails);
            }

            return i18n("d_of_d")
                .arg(item.fails)
                .arg(item.failLimit);
        }

        case Columns::NextAnnounce:
        {
            if (item.isTier || item.tier < 0)
            {
                return "";
            }

            if (item.status == ListItemStatus::updating
                || item.nextAnnounce.count() <= 0)
            {
                return "-";
            }

            std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(item.nextAnnounce);
            std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(item.nextAnnounce - hours_left);
            std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(item.nextAnnounce - hours_left - min_left);

            return QString("%1h %2m %3s")
                .arg(hours_left.count())
                .arg(min_left.count())
                .arg(sec_left.count());
        }
        }

        break;
    }
    case Qt::FontRole:
    {
        if (item.isTier)
        {
            QFont font;
            font.setBold(true);
            font.setPointSizeF(8);
            return font;
        }
        break;
    }
    case Qt::BackgroundRole:
    {
        if (item.isTier)
        {
            return QColor(245, 245, 245);
        }
        break;
    }
    case Qt::ForegroundRole:
    {
        if (item.isTier)
        {
            return QColor(Qt::gray);
        }
        break;
    }
    case Qt::TextAlignmentRole:
    {
        switch (index.column())
        {
        case Columns::Fails:
        case Columns::NextAnnounce:
        case Columns::NumDownloaded:
        case Columns::NumLeeches:
        case Columns::NumSeeds:
        {
            return (Qt::AlignVCenter | Qt::AlignRight);
        }
        }
    }
    case Qt::UserRole:
    {
        return QVariant::fromValue(item);
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

        case Columns::NumDownloaded:
            return i18n("downloaded");

        case Columns::NumLeeches:
            return i18n("leeches");

        case Columns::NumSeeds:
            return i18n("seeds");

        case Columns::Fails:
            return i18n("fails");

        case Columns::NextAnnounce:
            return i18n("next_announce");
        }

        break;
    }
    case Qt::TextAlignmentRole:
    {
        switch (section)
        {
        case Columns::Fails:
        case Columns::NextAnnounce:
        case Columns::NumDownloaded:
        case Columns::NumLeeches:
        case Columns::NumSeeds:
        {
            return (Qt::AlignVCenter | Qt::AlignRight);
        }
        }
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
