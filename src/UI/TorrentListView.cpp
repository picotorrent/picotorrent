#include "TorrentListView.hpp"

#include <filesystem>
#include <iomanip>

#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <picotorrent/api.hpp>

#include <commctrl.h>
#include <strsafe.h>

#include "../Clipboard.hpp"
#include "../Commands/MoveTorrentsCommand.hpp"
#include "../Commands/PauseTorrentsCommand.hpp"
#include "../Commands/QueueTorrentCommand.hpp"
#include "../Commands/RemoveTorrentsCommand.hpp"
#include "../Commands/ResumeTorrentsCommand.hpp"
#include "../Commands/ShowTorrentDetailsCommand.hpp"
#include "../Configuration.hpp"
#include "../Dialogs/OpenFileDialog.hpp"
#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"

#define LV_COL_NAME 0
#define LV_COL_QUEUE_POSITION 1
#define LV_COL_SIZE 2
#define LV_COL_STATUS 3
#define LV_COL_PROGRESS 4
#define LV_COL_ETA 5
#define LV_COL_DL 6
#define LV_COL_UL 7
#define LV_COL_RATIO 8
#define LV_COL_SEEDS 9
#define LV_COL_PEERS 10

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using UI::TorrentListView;

typedef std::function<bool(const Torrent&, const Torrent&)> sort_func_t;

TorrentListView::TorrentListView(
    HWND hWnd)
    : ListView::ListView(hWnd)
{
    AddColumn(
        std::wstring(TRW("name")),
        LV_COL_NAME,
        SX(280),
        LVCFMT_LEFT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("queue_position")),
        LV_COL_QUEUE_POSITION,
        SX(30),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("size")),
        LV_COL_SIZE,
        SX(80),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("status")),
        LV_COL_STATUS,
        SX(120),
        LVCFMT_LEFT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("progress")),
        LV_COL_PROGRESS,
        SX(100),
        LVCFMT_LEFT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT,
        true);

    AddColumn(
        std::wstring(TRW("eta")),
        LV_COL_ETA,
        SX(80),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("dl")),
        LV_COL_DL,
        SX(80),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("ul")),
        LV_COL_UL,
        SX(80),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("ratio")),
        LV_COL_RATIO,
        SX(80),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("seeds")),
        LV_COL_SEEDS,
        SX(80),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    AddColumn(
        std::wstring(TRW("peers")),
        LV_COL_PEERS,
        SX(80),
        LVCFMT_RIGHT,
        LVCF_TEXT | LVCF_WIDTH | LVCF_FMT);

    LoadState("torrents");

    // Load status icons
    m_icons = ImageList_Create(
        SX(16),
        SY(16),
        ILC_MASK | ILC_COLOR24,
        10,
        10);

    HBITMAP hbm = reinterpret_cast<HBITMAP>(LoadImage(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDB_STATUS_ICONS+SX(16)),
        IMAGE_BITMAP,
        0,
        0,
        0));

    ImageList_Add(m_icons, hbm, NULL);
    SetImageList(m_icons, LVSIL_SMALL);
}

TorrentListView::~TorrentListView()
{
    SaveState("torrents");
}

void TorrentListView::Add(const Torrent& model)
{
    m_models.push_back(model);
    SetItemCount((int)m_models.size());
}

std::vector<Torrent> TorrentListView::GetSelectedTorrents()
{
    std::vector<Torrent> selection;

    int pos = GetNextItem(-1, LVNI_SELECTED);
    while (pos != -1)
    {
        selection.push_back(m_models.at(pos));
        pos = GetNextItem(pos, LVNI_SELECTED);
    }

    return selection;
}

void TorrentListView::Remove(const std::string& infoHash)
{
	auto f = std::find_if(
		m_models.begin(),
		m_models.end(),
		[&infoHash](Torrent& t)
	{
		return t.infoHash == infoHash;
	});

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

int TorrentListView::GetItemIconIndex(int itemIndex)
{
    switch (m_models.at(itemIndex).state)
    {
    case Torrent::State::CheckingResumeData:
    case Torrent::State::DownloadingChecking:
    case Torrent::State::UploadingChecking:
        return 0;
    case Torrent::State::Complete:
        return 13;
    case Torrent::State::Downloading:
        return 1;
    case Torrent::State::DownloadingForced:
        return 2;
    case Torrent::State::DownloadingMetadata:
        return 3;
    case Torrent::State::DownloadingPaused:
        return 4;
    case Torrent::State::DownloadingQueued:
        return 5;
    case Torrent::State::DownloadingStalled:
        return 6;
    case Torrent::State::Error:
        return 7;
    case Torrent::State::Uploading:
        return 8;
    case Torrent::State::UploadingForced:
        return 9;
    case Torrent::State::UploadingPaused:
        return 10;
    case Torrent::State::UploadingQueued:
        return 11;
    case Torrent::State::UploadingStalled:
        return 12;
    }

    return -1;
}

float TorrentListView::GetItemProgress(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_PROGRESS:
        return m_models.at(itemIndex).progress;
    }

    return -1;
}

std::wstring TorrentListView::GetItemText(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_NAME:
        return TWS(m_models.at(itemIndex).name);
    case LV_COL_QUEUE_POSITION:
    {
        int qp = m_models.at(itemIndex).queuePosition;
        if (qp < 0) { return TEXT("-"); }
        return std::to_wstring(qp + 1);
    }
    case LV_COL_SIZE:
    {
        Torrent& t = m_models.at(itemIndex);

        if (t.totalSize < 0)
        {
            return L"-";
        }

        TCHAR wanted[100];
        StrFormatByteSize64(
            t.totalWanted,
            wanted,
            ARRAYSIZE(wanted));

        if (t.totalSize == t.totalWanted)
        {
            return wanted;
        }

        TCHAR total[100];
        StrFormatByteSize64(
            t.totalSize,
            total,
            ARRAYSIZE(total));

        TCHAR text[200];
        StringCchPrintf(
            text,
            ARRAYSIZE(text),
            TEXT("%s (%s)"),
            wanted,
            total);

        return text;
    }
    case LV_COL_STATUS:
    {
        switch (m_models.at(itemIndex).state)
        {
        case Torrent::State::CheckingResumeData:
            return TRW("state_checking_resume_data");
        case Torrent::State::Downloading:
            return TRW("state_downloading");
        case Torrent::State::DownloadingChecking:
            return TRW("state_downloading_checking");
        case Torrent::State::DownloadingForced:
            return TRW("state_downloading_forced");
        case Torrent::State::DownloadingMetadata:
            return TRW("state_downloading_metadata");
        case Torrent::State::DownloadingPaused:
            return TRW("state_downloading_paused");
        case Torrent::State::DownloadingQueued:
            return TRW("state_downloading_queued");
        case Torrent::State::DownloadingStalled:
            return TRW("state_downloading_stalled");
        case Torrent::State::Error:
            TCHAR err[1024];
            StringCchPrintf(
                err,
                ARRAYSIZE(err),
                TRW("state_error"),
                m_models.at(itemIndex).errorMessage.c_str());
            return err;
        case Torrent::State::Unknown:
            return TRW("state_unknown");
        case Torrent::State::Uploading:
            return TRW("state_uploading");
        case Torrent::State::UploadingChecking:
            return TRW("state_uploading_checking");
        case Torrent::State::UploadingForced:
            return TRW("state_uploading_forced");
        case Torrent::State::UploadingPaused:
            return TRW("state_uploading_paused");
        case Torrent::State::UploadingQueued:
            return TRW("state_uploading_queued");
        case Torrent::State::UploadingStalled:
            return TRW("state_uploading_stalled");
        }
        break;
    }
    case LV_COL_ETA:
    {
        std::chrono::seconds eta = m_models.at(itemIndex).eta;
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
            ? m_models.at(itemIndex).downloadRate
            : m_models.at(itemIndex).uploadRate;

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
        wss << std::fixed << std::setprecision(3) << m_models.at(itemIndex).shareRatio;
        return wss.str();
    }
    case LV_COL_SEEDS:
    {
        int num = m_models.at(itemIndex).seedsConnected;
        int list = m_models.at(itemIndex).seedsTotal;

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
        int num = m_models.at(itemIndex).peersConnected;
        int list = m_models.at(itemIndex).peersTotal;

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

void TorrentListView::OnItemActivated(std::vector<int> const& indices)
{
    if (indices.size() != 1)
    {
        return;
    }

    std::vector<Torrent> selection;
    std::for_each(
        indices.begin(),
        indices.end(),
        [this, &selection](int i) { selection.push_back(m_models.at(i)); });

    Commands::ShowTorrentDetailsCommand show{ selection[0] };
    SendCommand(PT_SHOWTORRENTDETAILS, reinterpret_cast<LPARAM>(&show));
}

void TorrentListView::ShowContextMenu(POINT p, const std::vector<int>& sel)
{
    if (sel.empty())
    {
        return;
    }

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

    AppendMenu(menu, MF_POPUP, (UINT_PTR)queue, TRW("queuing"));
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_REMOVE, TRW("remove_torrent"));
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_REMOVE_DATA, TRW("remove_torrent_and_files"));
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_COPY_SHA, TRW("copy_info_hash"));
    AppendMenu(menu, MF_STRING, TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER, TRW("open_in_explorer"));

    // Configure the menu
    bool allPaused = std::all_of(sel.begin(), sel.end(), [this](int i) { return m_models[i].isPaused; });
    bool allNotPaused = std::all_of(sel.begin(), sel.end(), [this](int i) { return !m_models[i].isPaused; });

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
        SendCommand(PT_PAUSETORRENTS, reinterpret_cast<LPARAM>(&pause));
        break;
    }
    case TORRENT_CONTEXT_MENU_RESUME:
    case TORRENT_CONTEXT_MENU_RESUME_FORCE:
    {
        Commands::ResumeTorrentsCommand resume{ res == TORRENT_CONTEXT_MENU_RESUME_FORCE, selection };
        SendCommand(PT_RESUMETORRENTS, reinterpret_cast<LPARAM>(&resume));
        break;
    }
    case TORRENT_CONTEXT_MENU_MOVE:
    {
        Commands::MoveTorrentsCommand move{ selection };
        SendCommand(PT_MOVETORRENTS, reinterpret_cast<LPARAM>(&move));
        break;
    }
    case TORRENT_CONTEXT_MENU_QUEUE_UP:
    {
        Commands::QueueTorrentCommand cmd{ Commands::QueueTorrentCommand::Up, m_models.at(sel[0]) };
        SendCommand(PT_QUEUETORRENT, reinterpret_cast<LPARAM>(&cmd));
        break;
    }
    case TORRENT_CONTEXT_MENU_QUEUE_DOWN:
    {
        Commands::QueueTorrentCommand cmd{ Commands::QueueTorrentCommand::Down, m_models.at(sel[0]) };
        SendCommand(PT_QUEUETORRENT, reinterpret_cast<LPARAM>(&cmd));
        break;
    }
    case TORRENT_CONTEXT_MENU_QUEUE_TOP:
    {
        Commands::QueueTorrentCommand cmd{ Commands::QueueTorrentCommand::Top, m_models.at(sel[0]) };
        SendCommand(PT_QUEUETORRENT, reinterpret_cast<LPARAM>(&cmd));
        break;
    }
    case TORRENT_CONTEXT_MENU_QUEUE_BOTTOM:
    {
        Commands::QueueTorrentCommand cmd{ Commands::QueueTorrentCommand::Bottom, m_models.at(sel[0]) };
        SendCommand(PT_QUEUETORRENT, reinterpret_cast<LPARAM>(&cmd));
        break;
    }
    case TORRENT_CONTEXT_MENU_REMOVE:
    case TORRENT_CONTEXT_MENU_REMOVE_DATA:
    {
        Commands::RemoveTorrentsCommand remove{ res == TORRENT_CONTEXT_MENU_REMOVE_DATA,selection };
        SendCommand(PT_REMOVETORRENTS, reinterpret_cast<LPARAM>(&remove));
        break;
    }
    case TORRENT_CONTEXT_MENU_COPY_SHA:
    {
        std::stringstream ss;
        std::for_each(sel.begin(), sel.end(),
            [this, &ss](int i)
        {
            ss << "," << m_models[i].infoHash;
        });

        Clipboard::Set(TWS(ss.str().substr(1)));
        break;
    }
    case TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER:
    {
        fs::path savePath = m_models.at(sel[0]).savePath;
        fs::path path = savePath / m_models.at(sel[0]).name;

        LPITEMIDLIST il = ILCreateFromPath(path.c_str());
        SHOpenFolderAndSelectItems(il, 0, 0, 0);
        ILFree(il);
        break;
    }
    case TORRENT_CONTEXT_MENU_DETAILS:
        Commands::ShowTorrentDetailsCommand show{ selection[0] };
        SendCommand(PT_SHOWTORRENTDETAILS, reinterpret_cast<LPARAM>(&show));
        break;
    }
}

bool TorrentListView::Sort(int columnId, ListView::SortOrder order)
{
    bool asc = order == SortOrder::Ascending;
    sort_func_t sorter;

    switch (columnId)
    {
    case LV_COL_NAME:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.name < t2.name;
            return t1.name > t2.name;
        };
        break;
    }
    case LV_COL_QUEUE_POSITION:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.queuePosition < t2.queuePosition;
            return t1.queuePosition > t2.queuePosition;
        };
        break;
    }
    case LV_COL_SIZE:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.totalWanted < t2.totalWanted;
            return t1.totalWanted > t2.totalWanted;
        };
        break;
    }
    case LV_COL_STATUS:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.state < t2.state;
            return t1.state > t2.state;
        };
        break;
    }
    case LV_COL_PROGRESS:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.progress < t2.progress;
            return t1.progress > t2.progress;
        };
        break;
    }
    case LV_COL_ETA:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.eta < t2.eta;
            return t1.eta > t2.eta;
        };
        break;
    }
    case LV_COL_DL:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.downloadRate < t2.downloadRate;
            return t1.downloadRate > t2.downloadRate;
        };
        break;
    }
    case LV_COL_UL:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.uploadRate < t2.uploadRate;
            return t1.uploadRate > t2.uploadRate;
        };
        break;
    }
    case LV_COL_SEEDS:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.seedsConnected + t1.seedsTotal < t2.seedsConnected + t2.seedsTotal;
            return t1.seedsConnected + t1.seedsTotal > t2.seedsConnected + t2.seedsTotal;
        };
        break;
    }
    case LV_COL_PEERS:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.peersConnected + t1.peersTotal < t2.peersConnected + t2.peersTotal;
            return t1.peersConnected + t1.peersTotal > t2.peersConnected + t2.peersTotal;
        };
        break;
    }
    case LV_COL_RATIO:
    {
        sorter = [asc](const Torrent& t1, const Torrent& t2)
        {
            if (asc) return t1.shareRatio < t2.shareRatio;
            return t1.shareRatio > t2.shareRatio;
        };
        break;
    }
    }

    if (sorter)
    {
        std::sort(m_models.begin(), m_models.end(), sorter);
        return true;
    }

    return false;
}
