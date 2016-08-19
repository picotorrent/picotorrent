#include "TorrentListView.hpp"

#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include <strsafe.h>

#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"

#define LV_COL_NAME 1
#define LV_COL_QUEUE_POSITION 2
#define LV_COL_SIZE 3
#define LV_COL_STATUS 4
#define LV_COL_PROGRESS 5
#define LV_COL_ETA 6
#define LV_COL_DL 7
#define LV_COL_UL 8
#define LV_COL_SEEDS 9
#define LV_COL_PEERS 10
#define LV_COL_RATIO 11

namespace lt = libtorrent;
using UI::TorrentListView;

TorrentListView::TorrentListView(
    HWND hWnd,
    const std::vector<lt::sha1_hash>& hashes,
    const std::map<libtorrent::sha1_hash, libtorrent::torrent_status>& torrents)
    : ListView::ListView(hWnd),
    m_hashes(hashes),
    m_torrents(torrents)
{
    AddColumn(LV_COL_NAME, TRW("name"), SX(280), UI::ListView::ColumnType::Text);
    AddColumn(LV_COL_QUEUE_POSITION, TRW("queue_position"), SX(30), UI::ListView::ColumnType::Number);
    AddColumn(LV_COL_SIZE, TRW("size"), SX(80), UI::ListView::ColumnType::Number);
    AddColumn(LV_COL_STATUS, TRW("status"), SX(120), UI::ListView::ColumnType::Text);
    AddColumn(LV_COL_PROGRESS, TRW("progress"), SX(100), UI::ListView::ColumnType::Progress);
    AddColumn(LV_COL_ETA, TRW("eta"), SX(80), UI::ListView::ColumnType::Number);
    AddColumn(LV_COL_DL, TRW("dl"), SX(80), UI::ListView::ColumnType::Number);
    AddColumn(LV_COL_UL, TRW("ul"), SX(80), UI::ListView::ColumnType::Number);
    AddColumn(LV_COL_RATIO, TRW("ratio"), SX(80), UI::ListView::ColumnType::Number);
    AddColumn(LV_COL_SEEDS, TRW("seeds"), SX(80), UI::ListView::ColumnType::Number);
    AddColumn(LV_COL_PEERS, TRW("peers"), SX(80), UI::ListView::ColumnType::Number);
}

std::wstring TorrentListView::GetItemText(int columnId, int itemIndex)
{
    const lt::sha1_hash& hash = m_hashes.at(itemIndex);
    const lt::torrent_status& ts = m_torrents.at(hash);

    switch (columnId)
    {
    case LV_COL_NAME:
    {
        return ToWideString(ts.name);
    }
    case LV_COL_QUEUE_POSITION:
    {
        if (ts.queue_position < 0) { return TEXT("-"); }
        return std::to_wstring(ts.queue_position + 1);
    }
    case LV_COL_SIZE:
    {
        TCHAR s[100];
        StrFormatByteSize64(ts.handle.torrent_file()->total_size(), s, ARRAYSIZE(s));
        return s;
    }
    case LV_COL_STATUS:
    {
        break;
    }
    case LV_COL_PROGRESS:
    {
        break;
    }
    case LV_COL_ETA:
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

        std::chrono::seconds next(eta);
        if (next.count() < 0) { return TEXT("-"); }

        std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(next);
        std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(next - hours_left);
        std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(next - hours_left - min_left);

        TCHAR t[100];
        StringCchPrintf(
            t,
            ARRAYSIZE(t),
            L"%dh %dm %ds",
            hours_left.count(),
            min_left.count(),
            sec_left.count());
        return t;
    }
    case LV_COL_DL:
    case LV_COL_UL:
    {
        int rate = columnId == LV_COL_DL
            ? ts.download_payload_rate
            : ts.upload_payload_rate;

        if (rate < 1024)
        {
            return TEXT("-");
        }

        TCHAR speed[128];
        StrFormatByteSize64(rate, speed, ARRAYSIZE(speed));
        StringCchPrintf(speed, ARRAYSIZE(speed), L"%s/s", speed);

        return speed;
    }
    case LV_COL_SEEDS:
    {
        TCHAR seeds[1024];
        StringCchPrintf(
            seeds,
            ARRAYSIZE(seeds),
            TEXT("%d (%d)"),
            ts.num_seeds,
            ts.list_seeds);
        return seeds;
    }
    case LV_COL_PEERS:
    {
        int num = ts.num_peers < ts.num_seeds;
        int list = ts.list_peers - ts.list_seeds;
        TCHAR peers[1024];
        StringCchPrintf(
            peers,
            ARRAYSIZE(peers),
            TEXT("%d (%d)"),
            std::max(num, 0),
            std::max(list, 0));
        return peers;
    }
    case LV_COL_RATIO:
    {
        int64_t ul = ts.all_time_upload;
        int64_t dl = ts.all_time_download;
        float ratio = 0;

        if (dl > 0) { ratio = (float)ul / (float)dl; }

        std::wstringstream wss;
        wss << std::fixed << std::setprecision(3) << ratio;
        return wss.str();
    }
    }

    return TEXT("?unknown?");
}
