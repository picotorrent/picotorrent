#include "TorrentListView.hpp"

#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include <strsafe.h>

#include "../Dialogs/OpenFileDialog.hpp"
#include "../IO/Path.hpp"
#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"
#include "../ViewModels/TorrentListViewModel.hpp"

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
using ViewModels::TorrentListViewModel;

TorrentListView::TorrentListView(
    HWND hWnd,
    std::unique_ptr<ViewModels::TorrentListViewModel> model)
    : ListView::ListView(hWnd)
{
    m_model = std::move(model);

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

float TorrentListView::GetItemProgress(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_PROGRESS:
        return m_model->GetProgress(itemIndex);
    }

    return -1;
}

std::wstring TorrentListView::GetItemText(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_NAME:
        return m_model->GetName(itemIndex);
    case LV_COL_QUEUE_POSITION:
    {
        int qp = m_model->GetQueuePosition(itemIndex);
        if (qp < 0) { return TEXT("-"); }
        return std::to_wstring(qp + 1);
    }
    case LV_COL_SIZE:
    {
        TCHAR s[100];
        StrFormatByteSize64(m_model->GetSize(itemIndex), s, ARRAYSIZE(s));
        return s;
    }
    case LV_COL_STATUS:
    {
        break;
    }
    case LV_COL_ETA:
    {
        std::chrono::seconds eta = m_model->GetETA(itemIndex);
        if (eta.count() < 0) { return TEXT("-"); }

        std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(eta);
        std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(eta - hours_left);
        std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(eta - hours_left - min_left);

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
            ? m_model->GetDownloadRate(itemIndex)
            : m_model->GetUploadRate(itemIndex);

        if (rate < 1024)
        {
            return TEXT("-");
        }

        TCHAR speed[128];
        StrFormatByteSize64(rate, speed, ARRAYSIZE(speed));
        StringCchPrintf(speed, ARRAYSIZE(speed), L"%s/s", speed);

        return speed;
    }
    case LV_COL_RATIO:
    {
        std::wstringstream wss;
        wss << std::fixed << std::setprecision(3) << m_model->GetShareRatio(itemIndex);
        return wss.str();
    }
    case LV_COL_SEEDS:
    {
        auto p = m_model->GetSeeds(itemIndex);
        TCHAR seeds[1024];
        StringCchPrintf(
            seeds,
            ARRAYSIZE(seeds),
            TEXT("%d (%d)"),
            p.first,
            p.second);
        return seeds;
    }
    case LV_COL_PEERS:
    {
        auto p = m_model->GetPeers(itemIndex);
        TCHAR peers[1024];
        StringCchPrintf(
            peers,
            ARRAYSIZE(peers),
            TEXT("%d (%d)"),
            std::max(p.first, 0),
            std::max(p.second, 0));
        return peers;
    }
    }

    return TEXT("?unknown?");
}

void TorrentListView::ShowContextMenu(POINT p, const std::vector<int>& sel)
{
    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_PAUSE, TRW("pause"));
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_RESUME, TRW("resume"));
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_RESUME_FORCE, TRW("resume_force"));
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_DETAILS, TRW("details"));
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_MOVE, TRW("move"));

    HMENU queue = CreateMenu();
    AppendMenu(queue, MF_STRING, TORRENT_CONTEXT_MENU_QUEUE_UP, TRW("up"));
    AppendMenu(queue, MF_STRING, TORRENT_CONTEXT_MENU_QUEUE_DOWN, TRW("down"));
    AppendMenu(queue, MF_SEPARATOR, 0, NULL);
    AppendMenu(queue, MF_STRING, TORRENT_CONTEXT_MENU_QUEUE_TOP, TRW("top"));
    AppendMenu(queue, MF_STRING, TORRENT_CONTEXT_MENU_QUEUE_BOTTOM, TRW("bottom"));

    HMENU remove = CreateMenu();
    AppendMenu(remove, MF_STRING, TORRENT_CONTEXT_MENU_REMOVE, TRW("remove_torrent"));
    AppendMenu(remove, MF_STRING, TORRENT_CONTEXT_MENU_REMOVE_DATA, TRW("remove_torrent_and_files"));

    AppendMenu(menu, MF_POPUP, (UINT_PTR)queue, TRW("queuing"));
    AppendMenu(menu, MF_POPUP, (UINT_PTR)remove, TRW("remove"));
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_COPY_SHA, TRW("copy_info_hash"));
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER, TRW("open_in_explorer"));

    // Configure the menu
    bool allPaused = std::all_of(sel.begin(), sel.end(), [this](int i) { return m_model->IsPaused(i); });
    bool allNotPaused = std::all_of(sel.begin(), sel.end(), [this](int i) { return !m_model->IsPaused(i); });

    if (allPaused)
    {
        RemoveMenu(menu, TORRENT_CONTEXT_MENU_PAUSE, MF_BYCOMMAND);
        SetMenuDefaultItem(menu, TORRENT_CONTEXT_MENU_RESUME, FALSE);
    }
    else if (allNotPaused)
    {
        RemoveMenu(menu, TORRENT_CONTEXT_MENU_RESUME, MF_BYCOMMAND);
        RemoveMenu(menu, TORRENT_CONTEXT_MENU_RESUME_FORCE, MF_BYCOMMAND);
        SetMenuDefaultItem(menu, TORRENT_CONTEXT_MENU_PAUSE, FALSE);
    }

    if (sel.size() > 1)
    {
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_QUEUE_UP, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_QUEUE_DOWN, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_QUEUE_TOP, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_QUEUE_BOTTOM, MF_BYCOMMAND | MF_DISABLED);
    }

    int res = TrackPopupMenu(
        menu,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        0,
        GetHandle(),
        NULL);
    
    switch (res)
    {
    case TORRENT_CONTEXT_MENU_PAUSE:
        std::for_each(sel.begin(), sel.end(), [this](int i) { m_model->Pause(i); });
        break;
    case TORRENT_CONTEXT_MENU_RESUME:
    case TORRENT_CONTEXT_MENU_RESUME_FORCE:
        std::for_each(sel.begin(), sel.end(), [this, res](int i) { m_model->Resume(i, res == TORRENT_CONTEXT_MENU_RESUME_FORCE); });
        break;
    case TORRENT_CONTEXT_MENU_MOVE:
    {
        Dialogs::OpenFileDialog dlg;
        dlg.SetOptions(dlg.GetOptions() | FOS_PICKFOLDERS | FOS_PATHMUSTEXIST);
        dlg.SetTitle(TRW("select_destination"));
        dlg.Show();

        auto paths = dlg.GetPaths();
        if (paths.size() == 0) { break; }

        std::for_each(sel.begin(), sel.end(), [this, paths](int i) { m_model->Move(i, paths[0]); });
        break;
    }
    case TORRENT_CONTEXT_MENU_QUEUE_UP:
        m_model->QueueUp(sel[0]);
        break;
    case TORRENT_CONTEXT_MENU_QUEUE_DOWN:
        m_model->QueueDown(sel[0]);
        break;
    case TORRENT_CONTEXT_MENU_QUEUE_TOP:
        m_model->QueueTop(sel[0]);
        break;
    case TORRENT_CONTEXT_MENU_QUEUE_BOTTOM:
        m_model->QueueBottom(sel[0]);
        break;
    case TORRENT_CONTEXT_MENU_REMOVE:
    case TORRENT_CONTEXT_MENU_REMOVE_DATA:
        break;
    case TORRENT_CONTEXT_MENU_COPY_SHA:
        break;
    case TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER:
    {
        std::wstring savePath = m_model->GetSavePath(sel[0]);
        std::wstring path = IO::Path::Combine(savePath, m_model->GetName(sel[0]));

        LPITEMIDLIST il = ILCreateFromPath(path.c_str());
        SHOpenFolderAndSelectItems(il, 0, 0, 0);
        ILFree(il);
        break;
    }
    case TORRENT_CONTEXT_MENU_DETAILS:
        break;
    }
}

bool TorrentListView::Sort(int columnId, ListView::SortOrder order)
{
    bool isAscending = order == SortOrder::Ascending;
    ViewModels::TorrentListViewModel& model = *m_model;

    switch (columnId)
    {
    case LV_COL_NAME:
        TorrentListViewModel::Sort::ByName(model, isAscending);
        return true;
    case LV_COL_QUEUE_POSITION:
        TorrentListViewModel::Sort::ByQueuePosition(model, isAscending);
        return true;
    case LV_COL_SIZE:
        TorrentListViewModel::Sort::BySize(model, isAscending);
        return true;
    case LV_COL_STATUS:
        TorrentListViewModel::Sort::ByStatus(model, isAscending);
        return true;
    case LV_COL_PROGRESS:
        TorrentListViewModel::Sort::ByProgress(model, isAscending);
        return true;
    case LV_COL_ETA:
        TorrentListViewModel::Sort::ByETA(model, isAscending);
        return true;
    case LV_COL_DL:
        TorrentListViewModel::Sort::ByDownloadRate(model, isAscending);
        return true;
    case LV_COL_UL:
        TorrentListViewModel::Sort::ByUploadRate(model, isAscending);
        return true;
    case LV_COL_SEEDS:
        TorrentListViewModel::Sort::BySeeds(model, isAscending);
        return true;
    case LV_COL_PEERS:
        TorrentListViewModel::Sort::ByPeers(model, isAscending);
        return true;
    case LV_COL_RATIO:
        TorrentListViewModel::Sort::ByShareRatio(model, isAscending);
        return true;
    }

    return false;
}
