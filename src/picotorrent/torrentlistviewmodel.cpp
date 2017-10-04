#include "torrentlistviewmodel.hpp"

#include "utils.hpp"

#include <libtorrent/torrent_status.hpp>

namespace lt = libtorrent;
using pt::TorrentListViewModel;

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

void TorrentListViewModel::Remove(lt::sha1_hash const& hash)
{
	m_status.erase(hash);

	auto it = std::find(m_order.begin(), m_order.end(), hash);
	auto d = std::distance(m_order.begin(), it);

	m_order.erase(it);
	RowDeleted(d);
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
	return -1;
}

wxString TorrentListViewModel::GetColumnType(unsigned int col) const
{
	return "string";
}

void TorrentListViewModel::GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const
{
	lt::sha1_hash const& hash = m_order.at(row);
	lt::torrent_status const& ts = m_status.at(hash);

	switch (col)
	{
	case 0:
		variant = ts.name;
		break;
	case 1:
		if (ts.queue_position < 0)
		{
			variant = "-";
		}
		else
		{
			variant = std::to_string(ts.queue_position + 1);
		}
		break;
	case 2:
		variant = Utils::ToHumanFileSize(ts.total_wanted);
		break;
	case 3:
		variant = "status";
		break;
	case 4:
		variant = static_cast<long>(ts.progress * 100);
		break;
	case 5:
	{
		if (ts.download_payload_rate <= 0)
		{
			variant = "-";
			break;
		}

		std::chrono::seconds secs(0);

		int64_t remaining_bytes = ts.total_wanted - ts.total_wanted_done;
		if (remaining_bytes > 0 && ts.download_payload_rate > 0)
		{
			secs = std::chrono::seconds(remaining_bytes / ts.download_payload_rate);
		}

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
	case 6:
	{
		if (ts.download_payload_rate <= 0)
		{
			variant = "-";
		}
		else
		{
			variant = wxString::Format(
				"%s/s",
				Utils::ToHumanFileSize(ts.download_payload_rate));
		}
		break;
	}
	case 7:
	{
		if (ts.upload_payload_rate <= 0)
		{
			variant = "-";
		}
		else
		{
			variant = wxString::Format(
				"%s/s",
				Utils::ToHumanFileSize(ts.upload_payload_rate));
		}
		break;
	}
	case 8:
	{
		float ratio = 0;

		if (ts.all_time_download > 0)
		{
			ratio = static_cast<float>(ts.all_time_upload) / static_cast<float>(ts.all_time_download);
		}

		variant = wxString::Format("%.3f", ratio);

		break;
	}
	case 9:
		variant = wxString::Format(
			"%d (of %d)",
			ts.num_seeds,
			ts.list_seeds);
		break;
	case 10:
		variant = wxString::Format(
			"%d (of %d)",
			ts.num_peers - ts.num_seeds,
			ts.list_peers - ts.list_seeds);
		break;
	}
}

bool TorrentListViewModel::SetValueByRow(const wxVariant &variant, unsigned row, unsigned col)
{
	return false;
}
