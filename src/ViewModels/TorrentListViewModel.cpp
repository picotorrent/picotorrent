#include "TorrentListViewModel.hpp"

#include <libtorrent/sha1.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

#include <algorithm>

#include "../StringUtils.hpp"

namespace lt = libtorrent;
using ViewModels::TorrentListViewModel;

void TorrentListViewModel::Sort::Apply(TorrentListViewModel& model, const std::function<bool(const lt::torrent_status& t1, const lt::torrent_status& t2)>& sorter)
{
    std::sort(model.m_hashes.begin(), model.m_hashes.end(),
        [model, sorter](const lt::sha1_hash& h1, const lt::sha1_hash& h2)
    {
        const lt::torrent_status& ts1 = model.m_torrents.at(h1);
        const lt::torrent_status& ts2 = model.m_torrents.at(h2);
        return sorter(ts1, ts2);
    });
}

void TorrentListViewModel::Sort::ByDownloadRate(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        if (ascending) { return ts1.download_payload_rate < ts2.download_payload_rate; }
        else { return ts1.download_payload_rate > ts2.download_payload_rate; }
    });
}

void TorrentListViewModel::Sort::ByETA(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [&model, ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        auto eta1 = model.CalculateETA(ts1);
        auto eta2 = model.CalculateETA(ts2);

        if (ascending) { return eta1 < eta2; }
        else { return eta1 > eta2; }
    });
}

void TorrentListViewModel::Sort::ByName(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        if (ascending) { return ts1.name < ts2.name; }
        else { return ts1.name > ts2.name; }
    });
}

void TorrentListViewModel::Sort::ByPeers(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        int n1 = ts1.num_peers - ts1.num_seeds;
        int n2 = ts2.num_peers - ts2.num_seeds;
        if (ascending) { return n1 < n2; }
        else { return n1 > n2; }
    });
}

void TorrentListViewModel::Sort::ByProgress(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        if (ascending) { return ts1.progress < ts2.progress; }
        else { return ts1.progress > ts2.progress; }
    });
}

void TorrentListViewModel::Sort::ByQueuePosition(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        if (ascending) { return ts1.queue_position < ts2.queue_position; }
        else { return ts1.queue_position > ts2.queue_position; }
    });
}

void TorrentListViewModel::Sort::BySeeds(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        int n1 = ts1.num_seeds;
        int n2 = ts2.num_seeds;
        if (ascending) { return n1 < n2; }
        else { return n1 > n2; }
    });
}

void TorrentListViewModel::Sort::BySize(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        int64_t t1s = ts1.handle.torrent_file()->total_size();
        int64_t t2s = ts2.handle.torrent_file()->total_size();

        if (ascending) { return t1s < t2s; }
        else { return t1s > t2s; }
    });
}

void TorrentListViewModel::Sort::ByShareRatio(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [&model, ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        float r1 = model.CalculateShareRatio(ts1);
        float r2 = model.CalculateShareRatio(ts2);

        if (ascending) { return r1 < r2; }
        else { return r1 > r2; }
    });
}

void TorrentListViewModel::Sort::ByStatus(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        if (ascending) { return ts1.state < ts2.state; }
        else { return ts1.state > ts2.state; }
    });
}

void TorrentListViewModel::Sort::ByUploadRate(TorrentListViewModel& model, bool ascending)
{
    Apply(model, [ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
    {
        if (ascending) { return ts1.upload_payload_rate < ts2.upload_payload_rate; }
        else { return ts1.upload_payload_rate > ts2.upload_payload_rate; }
    });
}

TorrentListViewModel::TorrentListViewModel(
    std::vector<lt::sha1_hash>& hashes,
    std::map<lt::sha1_hash, lt::torrent_status>& torrents)
    : m_hashes(hashes),
    m_torrents(torrents)
{
}

TorrentListViewModel::~TorrentListViewModel()
{
}

std::chrono::seconds TorrentListViewModel::CalculateETA(const lt::torrent_status& ts)
{
    bool is_paused = ts.paused && !ts.auto_managed;
    int eta = -1;

    if (!is_paused)
    {
        int64_t remaining_bytes = ts.total_wanted - ts.total_wanted_done;
        if (remaining_bytes > 0 && ts.download_payload_rate > 0)
        {
            eta = (int)(remaining_bytes / ts.download_payload_rate);
        }
    }

    return std::chrono::seconds(eta);
}

float TorrentListViewModel::CalculateShareRatio(const lt::torrent_status& ts)
{
    int64_t ul = ts.all_time_upload;
    int64_t dl = ts.all_time_download;
    float ratio = 0;

    if (dl > 0) { ratio = (float)ul / (float)dl; }

    return ratio;
}

int TorrentListViewModel::GetDownloadRate(int index)
{
    return GetStatus(index).download_payload_rate;
}

std::chrono::seconds TorrentListViewModel::GetETA(int index)
{
    lt::torrent_status& ts = GetStatus(index);
    return CalculateETA(ts);
}

std::wstring TorrentListViewModel::GetName(int index)
{
    return TWS(GetStatus(index).name);
}

std::pair<int, int> TorrentListViewModel::GetPeers(int index)
{
    lt::torrent_status& ts = GetStatus(index);
    return std::make_pair(
        ts.num_peers - ts.num_seeds,
        ts.list_peers - ts.list_seeds);
}

float TorrentListViewModel::GetProgress(int index)
{
    return GetStatus(index).progress;
}

int TorrentListViewModel::GetQueuePosition(int index)
{
    return GetStatus(index).queue_position;
}

std::wstring TorrentListViewModel::GetSavePath(int index)
{
    return TWS(GetStatus(index).save_path);
}

std::pair<int, int> TorrentListViewModel::GetSeeds(int index)
{
    lt::torrent_status& ts = GetStatus(index);
    return std::make_pair(
        ts.num_seeds,
        ts.list_seeds);
}

int64_t TorrentListViewModel::GetSize(int index)
{
    return GetStatus(index).handle.torrent_file()->total_size();
}

float TorrentListViewModel::GetShareRatio(int index)
{
    lt::torrent_status& ts = GetStatus(index);
    return CalculateShareRatio(ts);
}

lt::torrent_status& TorrentListViewModel::GetStatus(int index)
{
    const lt::sha1_hash& hash = m_hashes.at(index);
    return m_torrents.at(hash);
}

int TorrentListViewModel::GetUploadRate(int index)
{
    return GetStatus(index).upload_payload_rate;
}

bool TorrentListViewModel::IsPaused(int index)
{
    const lt::torrent_status& ts = GetStatus(index);
    return ts.paused && !ts.auto_managed;
}

void TorrentListViewModel::Move(int index, const std::wstring& path)
{
    GetStatus(index).handle.move_storage(ToString(path));
}

void TorrentListViewModel::Pause(int index)
{
    GetStatus(index).handle.pause();
}

void TorrentListViewModel::QueueUp(int index)
{
    GetStatus(index).handle.queue_position_up();
}

void TorrentListViewModel::QueueDown(int index)
{
    GetStatus(index).handle.queue_position_down();
}

void TorrentListViewModel::QueueTop(int index)
{
    GetStatus(index).handle.queue_position_top();
}

void TorrentListViewModel::QueueBottom(int index)
{
    GetStatus(index).handle.queue_position_bottom();
}

void TorrentListViewModel::Resume(int index, bool force)
{
    lt::torrent_status& ts = GetStatus(index);

    if (ts.paused && ts.errc)
    {
        ts.handle.clear_error();
    }

    ts.handle.set_upload_mode(false);
    ts.handle.auto_managed(!force);
    ts.handle.resume();
}
