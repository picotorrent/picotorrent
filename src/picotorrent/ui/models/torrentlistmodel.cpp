#include "torrentlistmodel.hpp"

#include <fmt/format.h>

#include "../../bittorrent/torrenthandle.hpp"
#include "../../bittorrent/torrentstatus.hpp"
#include "../../core/utils.hpp"
#include "../translator.hpp"

using pt::BitTorrent::TorrentHandle;
using pt::BitTorrent::TorrentStatus;
using pt::UI::Models::TorrentListModel;

TorrentListModel::TorrentListModel()
    : m_filter(nullptr)
{
}

TorrentListModel::~TorrentListModel()
{
}

void TorrentListModel::AddTorrent(BitTorrent::TorrentHandle* torrent)
{
    m_torrents.insert({ torrent->InfoHash(), torrent });

    if (m_filter == nullptr || m_filter(torrent))
    {
        m_filtered.push_back(torrent->InfoHash());
        RowAppended();
    }
}

void TorrentListModel::ClearFilter()
{
    m_filter = nullptr;
    m_filtered.clear();

    for (auto const& [hash, torrent] : m_torrents)
    {
        m_filtered.push_back(hash);
    }

    Reset(m_filtered.size());
}

void TorrentListModel::SetFilter(std::function<bool(BitTorrent::TorrentHandle*)> const& filter)
{
    m_filter = filter;
    ApplyFilter();
}

int TorrentListModel::GetRowIndex(TorrentHandle* torrent)
{
    return std::distance(
        m_filtered.begin(),
        std::find(
            m_filtered.begin(),
            m_filtered.end(), torrent->InfoHash()));
}

TorrentHandle* TorrentListModel::GetTorrentFromItem(wxDataViewItem const& item)
{
    uint32_t row = this->GetRow(item);
    auto const& hash = m_filtered.at(row);
    return m_torrents.at(hash);
}

void TorrentListModel::RemoveTorrent(lt::info_hash_t const& hash)
{
    m_torrents.erase(hash);

    auto iter = std::find(m_filtered.begin(), m_filtered.end(), hash);
    auto dist = std::distance(m_filtered.begin(), iter);

    m_filtered.erase(iter);

    RowDeleted(dist);
}

void TorrentListModel::UpdateTorrents(std::vector<TorrentHandle*> torrents)
{
    for (auto torrent : torrents)
    {
        auto iter = std::find(
            m_filtered.begin(),
            m_filtered.end(),
            torrent->InfoHash());

        auto dist = std::distance(
            m_filtered.begin(),
            iter);

        if (iter == m_filtered.end())
        {
            if ((m_filter && m_filter(torrent))
                || !m_filter)
            {
                m_filtered.push_back(torrent->InfoHash());
                RowAppended();
            }
        }
        else
        {
            if (m_filter && !m_filter(torrent))
            {
                m_filtered.erase(iter);
                RowDeleted(dist);
            }
            else if ((m_filter && m_filter(torrent))
                || !m_filter)
            {
                RowChanged(dist);
            }
        }
    }
}

int TorrentListModel::Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column, bool ascending) const
{
    auto const& hash1 = m_filtered.at(GetRow(item1));
    auto const& hash2 = m_filtered.at(GetRow(item2));

    auto const& lhs = m_torrents.at(hash1)->Status();
    auto const& rhs = m_torrents.at(hash2)->Status();

    auto hashSort = [](bool ascending, TorrentStatus const& l, TorrentStatus const& r) -> int
    {
        if (l.infoHash < r.infoHash) { return ascending ? -1 :  1; }
        if (l.infoHash > r.infoHash) { return ascending ?  1 : -1; }
        return 0;
    };

    switch (column)
    {
    case Columns::Name:
    {
        if (lhs.name < rhs.name)  { return ascending ? -1 :  1; }
        if (lhs.name > rhs.name)  { return ascending ?  1 : -1; }
        if (lhs.name == rhs.name) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::QueuePosition:
    {
        if (lhs.queuePosition < rhs.queuePosition)  { return ascending ? -1 :  1; }
        if (lhs.queuePosition > rhs.queuePosition)  { return ascending ?  1 : -1; }
        if (lhs.queuePosition == rhs.queuePosition) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::Size:
    {
        if (lhs.totalWanted < rhs.totalWanted) { return ascending ? -1 :  1; }
        if (lhs.totalWanted > rhs.totalWanted) { return ascending ?  1 : -1; }
        if (lhs.totalWanted == rhs.totalWanted) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::SizeRemaining:
    {
        if (lhs.totalWantedRemaining < rhs.totalWantedRemaining) { return ascending ? -1 : 1; }
        if (lhs.totalWantedRemaining > rhs.totalWantedRemaining) { return ascending ? 1 : -1; }
        if (lhs.totalWantedRemaining == rhs.totalWantedRemaining) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::Progress:
    {
        if (lhs.progress < rhs.progress) { return ascending ? -1 : 1; }
        if (lhs.progress > rhs.progress) { return ascending ? 1 : -1; }
        if (lhs.progress == rhs.progress) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::ETA:
    {
        if (lhs.eta < rhs.eta) { return ascending ? -1 : 1; }
        if (lhs.eta > rhs.eta) { return ascending ? 1 : -1; }
        if (lhs.eta == rhs.eta) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::DownloadSpeed:
    {
        if (lhs.downloadPayloadRate < rhs.downloadPayloadRate) { return ascending ? -1 : 1; }
        if (lhs.downloadPayloadRate > rhs.downloadPayloadRate) { return ascending ? 1 : -1; }
        if (lhs.downloadPayloadRate == rhs.downloadPayloadRate) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::UploadSpeed:
    {
        if (lhs.uploadPayloadRate < rhs.uploadPayloadRate) { return ascending ? -1 : 1; }
        if (lhs.uploadPayloadRate > rhs.uploadPayloadRate) { return ascending ? 1 : -1; }
        if (lhs.uploadPayloadRate == rhs.uploadPayloadRate) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::Availability:
    {
        if (lhs.availability < rhs.availability) { return ascending ? -1 : 1; }
        if (lhs.availability > rhs.availability) { return ascending ? 1 : -1; }
        if (lhs.availability == rhs.availability) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::Ratio:
    {
        if (lhs.ratio < rhs.ratio) { return ascending ? -1 : 1; }
        if (lhs.ratio > rhs.ratio) { return ascending ? 1 : -1; }
        if (lhs.ratio == rhs.ratio) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::AddedOn:
    {
        if (lhs.addedOn < rhs.addedOn) { return ascending ? -1 : 1; }
        if (lhs.addedOn > rhs.addedOn) { return ascending ? 1 : -1; }
        if (lhs.addedOn == rhs.addedOn) { return hashSort(ascending, lhs, rhs); }
    }
    case Columns::CompletedOn:
    {
        if (lhs.completedOn < rhs.completedOn) { return ascending ? -1 : 1; }
        if (lhs.completedOn > rhs.completedOn) { return ascending ? 1 : -1; }
        if (lhs.completedOn == rhs.completedOn) { return hashSort(ascending, lhs, rhs); }
    }
    }

    return 0;
}

wxString TorrentListModel::GetColumnType(unsigned int column) const
{
    return "string";
}

unsigned int TorrentListModel::GetCount() const
{
    return m_filtered.size();
}

void TorrentListModel::GetValueByRow(wxVariant& variant, uint32_t row, uint32_t col) const
{
    auto const& hash = m_filtered.at(row);
    BitTorrent::TorrentHandle* torrent = m_torrents.at(hash);
    BitTorrent::TorrentStatus  status = torrent->Status();

    switch (col)
    {
    case Columns::Name:
    {
        variant = status.name;
        break;
    }
    case Columns::QueuePosition:
    {
        variant = status.queuePosition < 0
            ? "-"
            : std::to_string(status.queuePosition + 1);
        break;
    }
    case Columns::Size:
    {
        variant = Utils::toHumanFileSize(status.totalWanted);
        break;
    }
    case Columns::SizeRemaining:
    {
        variant = status.totalWantedRemaining <= 0
            ? L"-"
            : Utils::toHumanFileSize(status.totalWantedRemaining);
        break;
    }
    case Columns::Status:
    {
        switch (status.state)
        {
        case TorrentStatus::State::CheckingFiles:
        case TorrentStatus::State::DownloadingChecking:
            variant = i18n("state_downloading_checking");
            break;

        case TorrentStatus::State::CheckingResumeData:
            variant = i18n("state_checking_resume_data");
            break;

        case TorrentStatus::State::Downloading:
            variant = i18n("state_downloading");
            break;

        case TorrentStatus::State::DownloadingMetadata:
            variant = i18n("state_downloading_metadata");
            break;

        case TorrentStatus::State::DownloadingPaused:
            variant = i18n("state_downloading_paused");
            break;

        case TorrentStatus::State::DownloadingQueued:
            variant = i18n("state_downloading_queued");
            break;

        case TorrentStatus::State::Error:
            variant = fmt::format(i18n("state_error"), Utils::toStdWString(status.error).c_str());
            break;

        case TorrentStatus::State::Unknown:
            variant = i18n("state_unknown");
            break;

        case TorrentStatus::State::Uploading:
            variant = i18n("state_uploading");
            break;

        case TorrentStatus::State::UploadingPaused:
            variant = i18n("state_uploading_paused");
            break;

        case TorrentStatus::State::UploadingQueued:
            variant = i18n("state_uploading_queued");
            break;
        default:
            variant = "-";
            break;
        }

        break;
    }
    case Columns::Progress:
    {
        variant = static_cast<long>(status.progress * 100);
        break;
    }
    case Columns::ETA:
    {
        variant = "-";

        if (status.paused || status.eta.count() <= 0)
        {
            break;
        }

        std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(status.eta);
        std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(status.eta - hours_left);
        std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(status.eta - hours_left - min_left);

        if (hours_left.count() <= 0)
        {
            if (min_left.count() <= 0)
            {
                variant = fmt::format("{0}s", sec_left.count());
                break;
            }

            variant = fmt::format("{0}m {1}s", min_left.count(), sec_left.count());
            break;
        }

        variant = fmt::format(
            "{0}h {1}m {2}s",
            hours_left.count(),
            min_left.count(),
            sec_left.count());

        break;
    }
    case Columns::DownloadSpeed:
    {
        variant = "-";

        if (status.paused || status.state == TorrentStatus::Uploading || status.downloadPayloadRate == 0)
        {
            break;
        }

        variant = fmt::format(L"{0}/s", Utils::toHumanFileSize(status.downloadPayloadRate));

        break;
    }
    case Columns::UploadSpeed:
    {
        variant = "-";

        if (status.paused || status.uploadPayloadRate == 0)
        {
            break;
        }

        variant = fmt::format(L"{0}/s", Utils::toHumanFileSize(status.uploadPayloadRate));

        break;
    }
    case Columns::Availability:
    {
        variant = "-";

        if (status.paused || status.availability < 0)
        {
            break;
        }

        variant = fmt::format("{:.3f}", status.availability);

        break;
    }
    case Columns::Ratio:
    {
        variant = fmt::format("{:.3f}", status.ratio);
        break;
    }
    case Columns::Seeds:
    {
        variant = "-";

        if (status.paused)
        {
            break;
        }

        variant = fmt::format(
            i18n("d_of_d"),
            status.seedsCurrent,
            status.seedsTotal);

        break;
    }
    case Columns::Peers:
    {
        variant = "-";

        if (status.paused)
        {
            break;
        }

        variant = fmt::format(
            i18n("d_of_d"),
            status.peersCurrent,
            status.peersTotal);

        break;
    }
    case Columns::AddedOn:
    {
        variant = wxDateTime(status.addedOn).FormatISOCombined(' ');
        break;
    }
    case Columns::CompletedOn:
    {
        variant = status.completedOn.IsValid()
            ? wxDateTime(status.completedOn).FormatISOCombined(' ')
            : "-";

        break;
    }
    }
}

bool TorrentListModel::ApplyFilter()
{
    if (m_filter)
    {
        for (auto const& [hash, torrent] : m_torrents)
        {
            auto isAdded = std::find(
                m_filtered.begin(),
                m_filtered.end(),
                hash);

            if (m_filter(torrent))
            {
                // Torrent should be included.
                if (isAdded == m_filtered.end())
                {
                    m_filtered.push_back(hash);
                    RowAppended();
                }
            }
            else
            {
                if (isAdded != m_filtered.end())
                {
                    auto distance = std::distance(m_filtered.begin(), isAdded);
                    m_filtered.erase(isAdded);
                    RowDeleted(distance);
                }
            }
        }

        return true;
    }

    return false;
}
