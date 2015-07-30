#include "torrentlistviewctrl.h"

#include <libtorrent/torrent_info.hpp>

namespace lt = libtorrent;

void CTorrentListViewCtrl::InsertTorrent(const libtorrent::torrent_status& status)
{
    if (items_.find(status.info_hash) != items_.end())
    {
        // ERROR
        // Adding same torrent twice.
        return;
    }
    
    int item = AddItem(GetItemCount(), 0, pt::to_lpwstr(status.name));
    AddItem(item, 1, pt::to_lpwstr(std::to_string(status.queue_position)));

    int64_t totalSize = -1;

    boost::shared_ptr<const lt::torrent_info> ti = status.torrent_file.lock();
    {
        if (ti)
        {
            totalSize = ti->total_size();
        }
    }

    AddItem(item, 2, pt::to_lpwstr(pt::to_file_size(totalSize)));

    char progress[100];
    snprintf(progress, sizeof(progress), "%.2f%%", status.progress * 100);
    AddItem(item, 3, pt::to_lpwstr(progress));
    
    AddItem(item, 4, pt::to_lpwstr(pt::to_state_string(status.state)));
    AddItem(item, 5, pt::to_lpwstr(pt::to_speed(status.download_payload_rate)));
    AddItem(item, 6, pt::to_lpwstr(pt::to_speed(status.upload_payload_rate)));

    items_[status.info_hash] = item;
}

void CTorrentListViewCtrl::UpdateTorrent(const libtorrent::torrent_status& status)
{
    if (items_.find(status.info_hash) == items_.end())
    {
        // ERROR
        // Not a torrent we have in our list.
        return;
    }

    int item = items_[status.info_hash];

    int64_t totalSize = -1;

    boost::shared_ptr<const lt::torrent_info> ti = status.torrent_file.lock();
    {
        if (ti)
        {
            totalSize = ti->total_size();
        }
    }

    SetItemText(item, 1, pt::to_lpwstr(std::to_string(status.queue_position)));
    SetItemText(item, 2, pt::to_lpwstr(pt::to_file_size(totalSize)));

    char progress[100];
    snprintf(progress, sizeof(progress), "%.2f%%", status.progress * 100);
    SetItemText(item, 3, pt::to_lpwstr(progress));

    SetItemText(item, 4, pt::to_lpwstr(pt::to_state_string(status.state)));
    SetItemText(item, 5, pt::to_lpwstr(pt::to_speed(status.download_payload_rate)));
    SetItemText(item, 6, pt::to_lpwstr(pt::to_speed(status.upload_payload_rate)));
}

LRESULT CTorrentListViewCtrl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    // TODO: Without the "DefWindowProc()" call, the list view does not show any columns.
    // This needs some explaining - but leaving it for now.
    DefWindowProc();

    SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

    InsertColumn(0, _T("Name"), LVCFMT_LEFT, 250, 0);
    InsertColumn(1, _T("#"), LVCFMT_RIGHT, 30, 1);
    InsertColumn(2, _T("Size"), LVCFMT_RIGHT, 80, 2);
    InsertColumn(3, _T("Progress"), LVCFMT_RIGHT, 100, 2);
    InsertColumn(4, _T("Status"), LVCFMT_LEFT, 140, 3);
    InsertColumn(5, _T("DL"), LVCFMT_RIGHT, 80, 4);
    InsertColumn(6, _T("UL"), LVCFMT_RIGHT, 80, 5);

    bHandled = FALSE;
    return 0;
}
