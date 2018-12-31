#include "torrentlistviewmodel.hpp"

#include "translator.hpp"
#include "utils.hpp"

#include <libtorrent/torrent_status.hpp>

namespace lt = libtorrent;
using pt::TorrentListViewModel;

TorrentListViewModel::TorrentListViewModel(std::shared_ptr<pt::Translator> translator)
    : m_translator(translator)
{
}

void TorrentListViewModel::Add(lt::torrent_status const& ts)
{
    m_order.push_back(ts.info_hash);
    m_status.insert({ ts.info_hash, ts });

    RowAppended();
}

lt::sha1_hash TorrentListViewModel::FindHashByRow(int row)
{
    return m_order.at(row);
}

int TorrentListViewModel::GetRowIndex(const lt::sha1_hash& hash)
{
    return std::distance(m_order.begin(), std::find(m_order.begin(), m_order.end(), hash));
}

void TorrentListViewModel::Remove(lt::sha1_hash const& hash)
{
    m_status.erase(hash);

    auto it = std::find(m_order.begin(), m_order.end(), hash);
    auto d = std::distance(m_order.begin(), it);

    m_order.erase(it);
    RowDeleted(d);
}

void TorrentListViewModel::Sort(int columnId, bool ascending)
{
    std::function<bool(lt::torrent_status const&, lt::torrent_status const&)> sorter;

    switch (columnId)
    {
    case Columns::Name:
    {
        sorter = [ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            if (ascending) { return ts1.name < ts2.name; }
            return ts1.name > ts2.name;
        };

        break;
    }
    case Columns::QueuePosition:
    {
        sorter = [ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            if (ascending) { return ts1.queue_position < ts2.queue_position; }
            return ts1.queue_position > ts2.queue_position;
        };

        break;
    }
    case Columns::Size:
    {
        sorter = [ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            if (ascending) { return ts1.total_wanted < ts2.total_wanted; }
            return ts1.total_wanted > ts2.total_wanted;
        };

        break;
    }
    case Columns::Progress:
    {
        sorter = [ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            if (ascending) { return ts1.progress < ts2.progress; }
            return ts1.progress > ts2.progress;
        };

        break;
    }
    case Columns::ETA:
    {
        sorter = [this, ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            auto secs1 = GetETA(ts1);
            auto secs2 = GetETA(ts2);

            if (ascending) { return secs1 < secs2; }
            return secs1 > secs2;
        };

        break;
    }
    case Columns::DownloadSpeed:
    {
        sorter = [ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            if (ascending) { return ts1.download_payload_rate < ts2.download_payload_rate; }
            return ts1.download_payload_rate > ts2.download_payload_rate;
        };

        break;
    }
    case Columns::UploadSpeed:
    {
        sorter = [ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            if (ascending) { return ts1.upload_payload_rate < ts2.upload_payload_rate; }
            return ts1.upload_payload_rate > ts2.upload_payload_rate;
        };

        break;
    }
    case Columns::Availability:
    {
        sorter = [this, ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            if (ascending) { return ts1.distributed_copies < ts2.distributed_copies; }
            return ts1.distributed_copies > ts2.distributed_copies;
        };

        break;
    }
    case Columns::Ratio:
    {
        sorter = [this, ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            float ratio1 = GetRatio(ts1);
            float ratio2 = GetRatio(ts2);

            if (ascending) { return ratio1 < ratio2; }
            return ratio1 > ratio2;
        };

        break;
    }
    case Columns::AddedOn:
    {
        sorter = [this, ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            if (ascending) { return ts1.added_time < ts2.added_time; }
            return ts1.added_time > ts2.added_time;
        };

        break;
    }
    case Columns::CompletedOn:
    {
        sorter = [this, ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            if (ascending) { return ts1.completed_time < ts2.completed_time; }
            return ts1.completed_time > ts2.completed_time;
        };

        break;
    }
    case Columns::Status:
    {
        sorter = [this, ascending](lt::torrent_status const& ts1, lt::torrent_status const& ts2)
        {
            wxString firstStatus = Utils::ToReadableStatus(ts1, m_translator);
            wxString secondStatus = Utils::ToReadableStatus(ts2, m_translator);
            if (ascending) { return firstStatus < secondStatus; }
            return firstStatus > secondStatus;
        };
        break;
    }
    }

    if (sorter)
    {
        std::sort(m_order.begin(), m_order.end(), [this, sorter](lt::sha1_hash& h1, lt::sha1_hash h2)
        {
            lt::torrent_status& ts1 = m_status.at(h1);
            lt::torrent_status& ts2 = m_status.at(h2);

            return sorter(ts1, ts2);
        });

        Reset(m_status.size());
    }
}

void TorrentListViewModel::Update(lt::torrent_status const& ts)
{
    m_status.at(ts.info_hash) = ts;

    auto it = std::find(m_order.begin(), m_order.end(), ts.info_hash);
    auto d = std::distance(m_order.begin(), it);

    RowChanged(d);
}

unsigned int TorrentListViewModel::GetColumnCount() const
{
    return Columns::_Max;
}

wxString TorrentListViewModel::GetColumnType(unsigned int col) const
{
    return "string";
}

void TorrentListViewModel::GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const
{
    lt::sha1_hash const& hash = m_order.at(row);
    lt::torrent_status const& ts = m_status.at(hash);
    bool isPaused = (ts.flags & lt::torrent_flags::paused) == lt::torrent_flags::paused;

    switch (col)
    {
    case Columns::Name:
        if (ts.name.empty())
        {
            std::stringstream ss;
            ss << ts.info_hash;
            variant = ss.str();
        }
        else
        {
            variant = wxString::FromUTF8(ts.name);
        }
        break;
    case Columns::QueuePosition:
        if (ts.queue_position == lt::queue_position_t{ -1 })
        {
            variant = "-";
        }
        else
        {
            variant = std::to_string(int(ts.queue_position) + 1);
        }
        break;
    case Columns::Size:
        variant = Utils::ToHumanFileSize(ts.total_wanted);
        break;
    case Columns::Status:
        variant = Utils::ToReadableStatus(ts, m_translator);
        break;
    case Columns::Progress:
        variant = static_cast<long>(ts.progress * 100);
        break;
    case Columns::ETA:
    {
        variant = "-";
        if (isPaused) { break; }

        std::chrono::seconds secs = GetETA(ts);
        if (secs.count() <= 0) { break; }

        std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(secs);
        std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(secs - hours_left);
        std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(secs - hours_left - min_left);

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

        if (ts.download_payload_rate > 0 && !isPaused)
        {
            variant = wxString::Format(
                "%s/s",
                Utils::ToHumanFileSize(ts.download_payload_rate));
        }
        break;
    }
    case Columns::UploadSpeed:
    {
        variant = "-";

        if (ts.upload_payload_rate > 0 && !isPaused)
        {
            variant = wxString::Format(
                "%s/s",
                Utils::ToHumanFileSize(ts.upload_payload_rate));
        }
        break;
    }
    case Columns::Availability:
    {
        variant = "-";

        if (ts.distributed_copies >= 0 && !isPaused)
        {
            variant = wxString::Format("%.3f", ts.distributed_copies);
        }
        break;
    }
    case Columns::Ratio:
    {
        float ratio = GetRatio(ts);
        variant = wxString::Format("%.3f", ratio);

        break;
    }
    case Columns::Seeds:
        if (isPaused)
        {
            variant = "-";
            break;
        }

        variant = wxString::Format(
            i18n(m_translator, "d_of_d"),
            ts.num_seeds,
            ts.list_seeds);
        break;
    case Columns::Peers:
        if (isPaused)
        {
            variant = "-";
            break;
        }

        variant = wxString::Format(
            i18n(m_translator, "d_of_d"),
            ts.num_peers - ts.num_seeds,
            ts.list_peers - ts.list_seeds);
        break;
    case Columns::AddedOn:
    {
        variant = wxDateTime(ts.added_time).FormatISOCombined(' ');
        break;
    }
    case Columns::CompletedOn:
    {
        if (ts.completed_time > 0)
        {
            variant = wxDateTime(ts.completed_time).FormatISOCombined(' ');
        }
        else
        {
            variant = "-";
        }
        break;
    }
    }
}

bool TorrentListViewModel::SetValueByRow(const wxVariant &variant, unsigned row, unsigned col)
{
    return false;
}

std::chrono::seconds TorrentListViewModel::GetETA(const lt::torrent_status& ts) const
{
    int64_t remaining_bytes = ts.total_wanted - ts.total_wanted_done;

    if (remaining_bytes > 0 && ts.download_payload_rate > 0)
    {
        return std::chrono::seconds(remaining_bytes / ts.download_payload_rate);
    }

    return std::chrono::seconds(0);
}

float TorrentListViewModel::GetRatio(const lt::torrent_status& ts) const
{
    if (ts.all_time_download > 0)
    {
        return static_cast<float>(ts.all_time_upload) / static_cast<float>(ts.all_time_download);
    }

    return 0;
}
