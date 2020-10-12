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
    : m_filter(nullptr),
    m_filterLabelId(-1)
{
}

TorrentListModel::~TorrentListModel()
{
}

void TorrentListModel::AddTorrent(BitTorrent::TorrentHandle* torrent)
{
    m_torrents.insert({ torrent->InfoHash(), torrent });
    ApplyFilter();
}

void TorrentListModel::ClearFilter()
{
    m_filter = nullptr;
}

void TorrentListModel::ClearLabelFilter()
{
    m_filterLabelId = -1;
    ApplyFilter();
}

void TorrentListModel::SetFilter(std::function<bool(BitTorrent::TorrentHandle*)> const& filter)
{
    m_filter = filter;
    ApplyFilter();
}

void TorrentListModel::SetLabelFilter(int labelId)
{
    m_filterLabelId = labelId;
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

    auto iter = std::find(
        m_filtered.begin(),
        m_filtered.end(),
        hash);

    if (iter != m_filtered.end())
    {
        auto dist = std::distance(m_filtered.begin(), iter);
        m_filtered.erase(iter);
        RowDeleted(dist);
    }
}

void TorrentListModel::UpdateTorrents(std::vector<TorrentHandle*> torrents)
{
    ApplyFilter(torrents);
}

void TorrentListModel::SetBackgroundColorEnabled(bool enabled)
{
    m_backgroundColorEnabled = enabled;
    Reset(m_filtered.size());
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

    auto nameSort = [&hashSort](bool ascending, TorrentStatus const& l, TorrentStatus const& r) -> int
    {
        if (l.name < r.name) { return ascending ? -1 : 1; }
        if (l.name > r.name) { return ascending ? 1 : -1; }
        return hashSort(ascending, l, l);
    };

    switch (column)
    {
    case Columns::Name:
    {
        return nameSort(ascending, lhs, rhs);
    }
    case Columns::QueuePosition:
    {
        if (lhs.queuePosition < rhs.queuePosition)  { return ascending ? -1 :  1; }
        if (lhs.queuePosition > rhs.queuePosition)  { return ascending ?  1 : -1; }
        if (lhs.queuePosition == rhs.queuePosition) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::Size:
    {
        if (lhs.totalWanted < rhs.totalWanted) { return ascending ? -1 :  1; }
        if (lhs.totalWanted > rhs.totalWanted) { return ascending ?  1 : -1; }
        if (lhs.totalWanted == rhs.totalWanted) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::SizeRemaining:
    {
        if (lhs.totalWantedRemaining < rhs.totalWantedRemaining) { return ascending ? -1 : 1; }
        if (lhs.totalWantedRemaining > rhs.totalWantedRemaining) { return ascending ? 1 : -1; }
        if (lhs.totalWantedRemaining == rhs.totalWantedRemaining) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::Progress:
    {
        if (lhs.progress < rhs.progress) { return ascending ? -1 : 1; }
        if (lhs.progress > rhs.progress) { return ascending ? 1 : -1; }
        if (lhs.progress == rhs.progress) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::ETA:
    {
        if (lhs.eta < rhs.eta) { return ascending ? -1 : 1; }
        if (lhs.eta > rhs.eta) { return ascending ? 1 : -1; }
        if (lhs.eta == rhs.eta) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::DownloadSpeed:
    {
        if (lhs.downloadPayloadRate < rhs.downloadPayloadRate) { return ascending ? -1 : 1; }
        if (lhs.downloadPayloadRate > rhs.downloadPayloadRate) { return ascending ? 1 : -1; }
        if (lhs.downloadPayloadRate == rhs.downloadPayloadRate) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::UploadSpeed:
    {
        if (lhs.uploadPayloadRate < rhs.uploadPayloadRate) { return ascending ? -1 : 1; }
        if (lhs.uploadPayloadRate > rhs.uploadPayloadRate) { return ascending ? 1 : -1; }
        if (lhs.uploadPayloadRate == rhs.uploadPayloadRate) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::Availability:
    {
        if (lhs.availability < rhs.availability) { return ascending ? -1 : 1; }
        if (lhs.availability > rhs.availability) { return ascending ? 1 : -1; }
        if (lhs.availability == rhs.availability) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::Ratio:
    {
        if (lhs.ratio < rhs.ratio) { return ascending ? -1 : 1; }
        if (lhs.ratio > rhs.ratio) { return ascending ? 1 : -1; }
        if (lhs.ratio == rhs.ratio) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::AddedOn:
    {
        if (lhs.addedOn < rhs.addedOn) { return ascending ? -1 : 1; }
        if (lhs.addedOn > rhs.addedOn) { return ascending ? 1 : -1; }
        if (lhs.addedOn == rhs.addedOn) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::CompletedOn:
    {
        if (lhs.completedOn < rhs.completedOn) { return ascending ? -1 : 1; }
        if (lhs.completedOn > rhs.completedOn) { return ascending ? 1 : -1; }
        if (lhs.completedOn == rhs.completedOn) { return hashSort(ascending, lhs, rhs); }
        break;
    }
    case Columns::Label:
    {
        if (lhs.labelName < rhs.labelName) { return ascending ? -1 : 1; }
        if (lhs.labelName > rhs.labelName) { return ascending ? 1 : -1; }
        if (lhs.labelName == rhs.labelName) { return nameSort(ascending, lhs, rhs); }
        break;
    }
    }

    return 0;
}

bool TorrentListModel::GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr& attr) const
{
    auto const& hash = m_filtered.at(row);
    BitTorrent::TorrentHandle* torrent = m_torrents.at(hash);

    // torrent has a label and a color
    if (torrent->Label() > 0
        && m_labelsColors.find(torrent->Label()) != m_labelsColors.end()
        && m_backgroundColorEnabled)
    {
        attr.SetBackgroundColour(
            m_labelsColors.at(
                torrent->Label()));
    }

    switch (col)
    {
    case Columns::Status:
    {
        BitTorrent::TorrentStatus  status = torrent->Status();

        if (status.state == TorrentStatus::State::Error)
        {
            attr.SetColour(*wxRED);
            return true;
        }

        return false;
    }
    }

    return false;
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
        variant = Utils::toStdWString(status.name);
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
            if (status.errorDetails.empty())
            {
                variant = fmt::format(
                    i18n("state_error"),
                    Utils::toStdWString(status.error).c_str());
            }
            else
            {
                variant = fmt::format(
                    i18n("state_error_details"),
                    Utils::toStdWString(status.error).c_str(),
                    Utils::toStdWString(status.errorDetails).c_str());
            }

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
    case Columns::Label:
    {
        if (torrent->Label() < 0)
        {
            variant << wxDataViewIconText("-");
            break;
        }

        wxIcon ic = wxNullIcon;
        auto [name, _] = m_labels.at(torrent->Label());

        if (m_labelsIcons.find(torrent->Label()) != m_labelsIcons.end())
        {
            ic = m_labelsIcons.at(torrent->Label());
        }

        variant << wxDataViewIconText(
            Utils::toStdWString(name), ic);

        break;
    }
    }
}

void TorrentListModel::UpdateLabels(std::map<int, std::tuple<std::string, std::string>> const& labels)
{
    std::vector<BitTorrent::TorrentHandle*> torrents;

    for (auto const& infoHash : m_filtered)
    {
        // if this torrent has a label which have changed color, we need to update it
        auto torrent = m_torrents.at(infoHash);

        // skip torrent if no label
        if (torrent->Label() < 0) { continue; }

        auto oldLabel = m_labels.find(torrent->Label());
        auto label = labels.find(torrent->Label());

        // color has changed
        if (oldLabel != m_labels.end()
            && label != labels.end()
            && oldLabel->second != label->second)
        {
            torrents.push_back(torrent);
            continue;
        }

        // color was removed
        if (oldLabel != m_labels.end()
            && label == labels.end())
        {
            torrents.push_back(torrent);
            continue;
        }

        // color was added
        if (oldLabel == m_labels.end()
            && label != labels.end())
        {
            torrents.push_back(torrent);
            continue;
        }
    }

    m_labels = labels;
    m_labelsColors.clear();
    m_labelsIcons.clear();

    for (auto const& [id, nv] : m_labels)
    {
        auto [name, color] = nv;

        if (color.empty()) { continue; }

        m_labelsColors.insert({ id, wxColor(color) });

        wxBitmap bmp(24, 24);

        {
            wxMemoryDC dc;
            wxDCBrushChanger dcbc(dc, m_labelsColors.at(id));
            dc.SelectObject(bmp);
            dc.Clear();
        }

        wxIcon ic;
        ic.CopyFromBitmap(bmp);

        m_labelsIcons.insert({ id, ic });
    }

    for (auto const& torrent : torrents)
    {
        auto iter = std::find(
            m_filtered.begin(),
            m_filtered.end(),
            torrent->InfoHash());

        auto dist = std::distance(
            m_filtered.begin(),
            iter);

        RowChanged(dist);
    }
}

void TorrentListModel::ApplyFilter()
{
    std::vector<TorrentHandle*> filter;
    for (auto const& [hash, torrent] : m_torrents)
    {
        filter.push_back(torrent);
    }
    ApplyFilter(filter);
}

void TorrentListModel::ApplyFilter(std::vector<BitTorrent::TorrentHandle*> torrents)
{
    const std::function<bool(TorrentHandle*)> show = [this](TorrentHandle* torrent)
    {
        // if both label id and filter function is set - this function must check that
        // the torrent both has the label and is included in the filter function
        // otherwise, check each
        if (m_filter && m_filterLabelId > 0)
        {
            return m_filter(torrent) && torrent->Label() == m_filterLabelId;
        }
        else if (m_filter)
        {
            return m_filter(torrent);
        }
        else if (m_filterLabelId > 0)
        {
            return torrent->Label() == m_filterLabelId;
        }

        return true;
    };

    for (auto torrent : torrents)
    {
        auto iter = std::find(
            m_filtered.begin(),
            m_filtered.end(),
            torrent->InfoHash());

        auto dist = std::distance(
            m_filtered.begin(),
            iter);

        // the torrent is not in the list of filtered torrents
        if (iter == m_filtered.end())
        {
            // but we want to show it according to the filters
            if (show(torrent))
            {
                // so show it
                m_filtered.push_back(torrent->InfoHash());
                RowAppended();
            }
        }
        // the torrent *is* in the list of filtered torrents
        else
        {
            // but we don't want to show it
            if (!show(torrent))
            {
                // so delete it
                m_filtered.erase(iter);
                RowDeleted(dist);
            }
            // and we still want to show it
            else
            {
                // so update it
                RowChanged(dist);
            }
        }
    }
}
