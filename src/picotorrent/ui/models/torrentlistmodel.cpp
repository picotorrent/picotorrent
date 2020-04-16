#include "torrentlistmodel.hpp"

#include "../../bittorrent/torrenthandle.hpp"
#include "../../bittorrent/torrentstatus.hpp"
#include "../../core/utils.hpp"
#include "../translator.hpp"

using pt::BitTorrent::TorrentHandle;
using pt::BitTorrent::TorrentStatus;
using pt::UI::Models::TorrentListModel;

TorrentListModel::TorrentListModel()
{
}

TorrentListModel::~TorrentListModel()
{
}

void TorrentListModel::AddTorrent(BitTorrent::TorrentHandle* torrent)
{
    m_order.push_back(torrent->InfoHash());
    m_torrents.insert({ torrent->InfoHash(), torrent });
    RowAppended();
}

int TorrentListModel::GetRowIndex(TorrentHandle* torrent)
{
    return std::distance(
        m_order.begin(),
        std::find(
            m_order.begin(),
            m_order.end(), torrent->InfoHash()));
}

TorrentHandle* TorrentListModel::GetTorrentFromItem(wxDataViewItem const& item)
{
    uint32_t row = this->GetRow(item);
    auto const& hash = m_order.at(row);
    return m_torrents.at(hash);
}

void TorrentListModel::RemoveTorrent(lt::info_hash_t const& hash)
{
    m_torrents.erase(hash);

    auto iter = std::find(m_order.begin(), m_order.end(), hash);
    auto dist = std::distance(m_order.begin(), iter);

    m_order.erase(iter);

    RowDeleted(dist);
}

void TorrentListModel::UpdateTorrent(TorrentHandle* torrent)
{
    m_torrents.at(torrent->InfoHash()) = torrent;

    auto iter = std::find(
        m_order.begin(),
        m_order.end(),
        torrent->InfoHash());

    auto dist = std::distance(
        m_order.begin(),
        iter);

    RowChanged(dist);
}


void TorrentListModel::Sort(uint32_t columnId, bool ascending)
{
    std::function<bool(TorrentStatus const&, TorrentStatus const&)> sorter;

    switch (columnId)
    {
    case Columns::Name:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.name < ts2.name; }
            return ts1.name > ts2.name;
        };

        break;
    }
    case Columns::QueuePosition:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.queuePosition < ts2.queuePosition; }
            return ts1.queuePosition > ts2.queuePosition;
        };

        break;
    }
    case Columns::Size:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.totalWanted < ts2.totalWanted; }
            return ts1.totalWanted > ts2.totalWanted;
        };

        break;
    }
    case Columns::Progress:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.progress < ts2.progress; }
            return ts1.progress > ts2.progress;
        };

        break;
    }
    case Columns::ETA:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.eta < ts2.eta; }
            return ts1.eta > ts2.eta;
        };

        break;
    }
    case Columns::DownloadSpeed:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.downloadPayloadRate < ts2.downloadPayloadRate; }
            return ts1.downloadPayloadRate > ts2.downloadPayloadRate;
        };

        break;
    }
    case Columns::UploadSpeed:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.uploadPayloadRate < ts2.uploadPayloadRate; }
            return ts1.uploadPayloadRate > ts2.uploadPayloadRate;
        };

        break;
    }
    case Columns::Availability:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.availability < ts2.availability; }
            return ts1.availability > ts2.availability;
        };

        break;
    }
    case Columns::Ratio:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.ratio < ts2.ratio; }
            return ts1.ratio > ts2.ratio;
        };

        break;
    }
    case Columns::AddedOn:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.addedOn < ts2.addedOn; }
            return ts1.addedOn > ts2.addedOn;
        };

        break;
    }
    case Columns::CompletedOn:
    {
        sorter = [ascending](auto const& ts1, auto const& ts2)
        {
            if (ascending) { return ts1.completedOn < ts2.completedOn; }
            return ts1.completedOn > ts2.completedOn;
        };

        break;
    }
    }

    if (sorter)
    {
        std::sort(
            m_order.begin(),
            m_order.end(),
            [this, sorter](lt::info_hash_t& h1, lt::info_hash_t h2)
            {
                auto const& ts1 = m_torrents.at(h1)->Status();
                auto const& ts2 = m_torrents.at(h2)->Status();

                return sorter(ts1, ts2);
            });

        Reset(m_torrents.size());
    }
}

wxString TorrentListModel::GetColumnType(unsigned int column) const
{
    return "string";
}

unsigned int TorrentListModel::GetCount() const
{
    return m_torrents.size();
}

void TorrentListModel::GetValueByRow(wxVariant& variant, uint32_t row, uint32_t col) const
{
    auto const& hash = m_order.at(row);
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
            variant = wxString::Format(wxString(i18n("state_error")), status.error.c_str());
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
                variant = wxString::Format("%I64ds", sec_left.count());
                break;
            }

            variant = wxString::Format("%dm %I64ds", min_left.count(), sec_left.count());
            break;
        }

        variant = wxString::Format(
            "%dh %dm %I64ds",
            hours_left.count(),
            min_left.count(),
            sec_left.count());

        break;
    }
    case Columns::DownloadSpeed:
    {
        variant = "-";

        if (status.paused || status.state == TorrentStatus::Uploading)
        {
            break;
        }

        variant = wxString::Format(
            "%s/s",
            Utils::toHumanFileSize(status.downloadPayloadRate));

        break;
    }
    case Columns::UploadSpeed:
    {
        variant = "-";

        if (status.paused)
        {
            break;
        }

        variant = wxString::Format(
            "%s/s",
            Utils::toHumanFileSize(status.uploadPayloadRate));

        break;
    }
    case Columns::Availability:
    {
        variant = "-";

        if (status.paused || status.availability < 0)
        {
            break;
        }

        variant = wxString::Format("%.3f", status.availability);

        break;
    }
    case Columns::Ratio:
    {
        variant = wxString::Format("%.3f", status.ratio);
        break;
    }
    case Columns::Seeds:
    {
        variant = "-";

        if (status.paused)
        {
            break;
        }

        variant = wxString::Format(
            wxString(i18n("d_of_d")),
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

        variant = wxString::Format(
            wxString(i18n("d_of_d")),
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
