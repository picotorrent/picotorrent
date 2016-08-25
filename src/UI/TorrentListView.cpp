#include "TorrentListView.hpp"

#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include <commctrl.h>
#include <strsafe.h>

#include "../Clipboard.hpp"
#include "../Commands/MoveTorrentsCommand.hpp"
#include "../Commands/PauseTorrentsCommand.hpp"
#include "../Commands/RemoveTorrentsCommand.hpp"
#include "../Commands/ResumeTorrentsCommand.hpp"
#include "../Commands/ShowTorrentDetailsCommand.hpp"
#include "../Configuration.hpp"
#include "../Dialogs/OpenFileDialog.hpp"
#include "../IO/Path.hpp"
#include "../Models/Torrent.hpp"
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
using Models::Torrent;
using UI::TorrentListView;

TorrentListView::TorrentListView(
    HWND hWnd)
    : ListView::ListView(hWnd)
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

void TorrentListView::Add(const Torrent& model)
{
    m_models.push_back(model);
    SetItemCount((int)m_models.size());

}

void TorrentListView::Remove(const Torrent& model)
{
    auto f = std::find(m_models.begin(), m_models.end(), model);
    if (f == m_models.end()) { return; }

    m_models.erase(f);
    SetItemCount((int)m_models.size());
}

void TorrentListView::Update(const Torrent& model)
{
    auto f = std::find(m_models.begin(), m_models.end(), model);

    if (f != m_models.end())
    {
        auto index = std::distance(m_models.begin(), f);
        m_models.at(index) = model;
    }
}

float TorrentListView::GetItemProgress(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_PROGRESS:
        return m_models.at(itemIndex).progress();
    }

    return -1;
}

std::wstring TorrentListView::GetItemText(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_NAME:
        return m_models.at(itemIndex).name();
    case LV_COL_QUEUE_POSITION:
    {
        int qp = m_models.at(itemIndex).queuePosition();
        if (qp < 0) { return TEXT("-"); }
        return std::to_wstring(qp + 1);
    }
    case LV_COL_SIZE:
    {
        TCHAR s[100];
        StrFormatByteSize64(m_models.at(itemIndex).size(), s, ARRAYSIZE(s));
        return s;
    }
    case LV_COL_STATUS:
    {
        break;
    }
    case LV_COL_ETA:
    {
        std::chrono::seconds eta(m_models.at(itemIndex).eta());
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
            ? m_models.at(itemIndex).downloadRate()
            : m_models.at(itemIndex).uploadRate();

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
        wss << std::fixed << std::setprecision(3) << m_models.at(itemIndex).shareRatio();
        return wss.str();
    }
    case LV_COL_SEEDS:
    {
        int num = m_models.at(itemIndex).seedsConnected();
        int list = m_models.at(itemIndex).seedsTotal();

        TCHAR seeds[1024];
        StringCchPrintf(
            seeds,
            ARRAYSIZE(seeds),
            TEXT("%d (%d)"),
            num,
            list);
        return seeds;
    }
    case LV_COL_PEERS:
    {
        int num = m_models.at(itemIndex).peersConnected();
        int list = m_models.at(itemIndex).peersTotal();

        TCHAR peers[1024];
        StringCchPrintf(
            peers,
            ARRAYSIZE(peers),
            TEXT("%d (%d)"),
            std::max(num, 0),
            std::max(list, 0));
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
    bool allPaused = std::all_of(sel.begin(), sel.end(), [this](int i) { return m_models[i].isPaused(); });
    bool allNotPaused = std::all_of(sel.begin(), sel.end(), [this](int i) { return !m_models[i].isPaused(); });

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

    // If we select more than one torrent, we disable some items which doesn't make
    // a lot of sense for more than one torrent.
    if (sel.size() > 1)
    {
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_QUEUE_UP, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_QUEUE_DOWN, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_QUEUE_TOP, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_QUEUE_BOTTOM, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(menu, TORRENT_CONTEXT_MENU_DETAILS, MF_BYCOMMAND | MF_DISABLED);
    }

    int res = TrackPopupMenu(
        menu,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        0,
        m_hWnd,
        NULL);

    std::vector<Torrent> selection;
    std::for_each(sel.begin(), sel.end(), [this, &selection](int i) { selection.push_back(m_models.at(i)); });

    switch (res)
    {
    case TORRENT_CONTEXT_MENU_PAUSE:
    {
        Commands::PauseTorrentsCommand pause{ selection };
        SendCommand(PT_PAUSETORRENTS, (LPARAM)&pause);
        break;
    }
    case TORRENT_CONTEXT_MENU_RESUME:
    case TORRENT_CONTEXT_MENU_RESUME_FORCE:
    {
        Commands::ResumeTorrentsCommand resume{ res == TORRENT_CONTEXT_MENU_RESUME_FORCE, selection };
        SendCommand(PT_RESUMETORRENTS, (LPARAM)&resume);
        break;
    }
    case TORRENT_CONTEXT_MENU_MOVE:
    {
        Commands::MoveTorrentsCommand move{ selection };
        SendCommand(PT_MOVETORRENTS, (LPARAM)&move);
        break;
    }
    case TORRENT_CONTEXT_MENU_QUEUE_UP:
        // m_model->QueueUp(sel[0]);
        break;
    case TORRENT_CONTEXT_MENU_QUEUE_DOWN:
        // m_model->QueueDown(sel[0]);
        break;
    case TORRENT_CONTEXT_MENU_QUEUE_TOP:
        // m_model->QueueTop(sel[0]);
        break;
    case TORRENT_CONTEXT_MENU_QUEUE_BOTTOM:
        // m_model->QueueBottom(sel[0]);
        break;
    case TORRENT_CONTEXT_MENU_REMOVE:
    case TORRENT_CONTEXT_MENU_REMOVE_DATA:
    {
        Commands::RemoveTorrentsCommand remove{ res == TORRENT_CONTEXT_MENU_REMOVE_DATA,selection };
        SendCommand(PT_REMOVETORRENTS, (LPARAM)&remove);
        break;
    }
    case TORRENT_CONTEXT_MENU_COPY_SHA:
    {
        std::wstringstream ss;
        std::for_each(sel.begin(), sel.end(),
            [this, &ss](int i)
        {
            ss << L"," << TWS(lt::to_hex(m_models[i].infoHash().to_string()));
        });

        Clipboard::Set(ss.str().substr(1));
        break;
    }
    case TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER:
    {
        std::wstring savePath = m_models.at(sel[0]).savePath();
        std::wstring path = IO::Path::Combine(savePath, m_models.at(sel[0]).name());

        LPITEMIDLIST il = ILCreateFromPath(path.c_str());
        SHOpenFolderAndSelectItems(il, 0, 0, 0);
        ILFree(il);
        break;
    }
    case TORRENT_CONTEXT_MENU_DETAILS:
        Commands::ShowTorrentDetailsCommand show{ selection[0] };
        SendCommand(PT_SHOWTORRENTDETAILS, (LPARAM)&show);
        break;
    }
}

bool TorrentListView::Sort(int columnId, ListView::SortOrder order)
{
    /*bool isAscending = order == SortOrder::Ascending;
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
    */
    return false;
}
