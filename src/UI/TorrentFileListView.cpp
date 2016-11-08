#include "TorrentFileListView.hpp"

#include "../Commands/PrioritizeFilesCommand.hpp"
#include "../Models/TorrentFile.hpp"
#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"

#define LV_COL_NAME 0
#define LV_COL_SIZE 1
#define LV_COL_PROGRESS 2
#define LV_COL_PRIO 3

using UI::TorrentFileListView;

TorrentFileListView::TorrentFileListView(HWND hWnd, bool showProgress)
    : ListView::ListView(hWnd),
    m_showProgress(showProgress)
{
    AddColumn(LV_COL_NAME, TRW("name"), SX(270), ColumnType::Text);
    AddColumn(LV_COL_SIZE, TRW("size"), SX(80), ColumnType::Number);

    if (showProgress)
    {
        AddColumn(LV_COL_PROGRESS, TRW("progress"), SX(120), ColumnType::Progress);
    }

    AddColumn(LV_COL_PRIO, TRW("priority"), SX(120), ColumnType::Text);

    // Enable checkboxes
    SetExtendedListViewStyle(GetExtendedListViewStyle() | LVS_EX_CHECKBOXES);
}

TorrentFileListView::~TorrentFileListView()
{
}

void TorrentFileListView::Add(const Models::TorrentFile& file)
{
    m_models.push_back(file);
}

void TorrentFileListView::RemoveAll()
{
    m_models.clear();
}

void TorrentFileListView::Update(const Models::TorrentFile& model)
{
    auto f = std::find(m_models.begin(), m_models.end(), model);

    if (f != m_models.end())
    {
        auto index = std::distance(m_models.begin(), f);
        m_models.at(index) = model;
    }
}

bool TorrentFileListView::GetItemIsChecked(int itemIndex)
{
    if (m_models.at(itemIndex).priority == PRIORITY_DO_NOT_DOWNLOAD)
    {
        return false;
    }

    return true;
}

float TorrentFileListView::GetItemProgress(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_PROGRESS:
        return m_models.at(itemIndex).progress;
    }

    return -1;
}

std::wstring TorrentFileListView::GetItemText(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_NAME:
        return m_models.at(itemIndex).name;
    case LV_COL_SIZE:
        TCHAR s[1024];
        StrFormatByteSize64(m_models.at(itemIndex).size, s, ARRAYSIZE(s));
        return s;
    case LV_COL_PRIO:
        return GetPriorityString(m_models.at(itemIndex).priority);
    }

    return L"?unknown column?";
}

void TorrentFileListView::ShowContextMenu(POINT p, const std::vector<int>& selectedIndices)
{
    if (selectedIndices.empty())
    {
        return;
    }

    HMENU prioMenu = CreateMenu();
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_MAX, TRW("maximum"));
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_HIGH, TRW("high"));
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_NORMAL, TRW("normal"));

    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_POPUP, (UINT_PTR)prioMenu, TRW("priority"));

    // If only one file is selected, check that files priority
    if (selectedIndices.size() == 1)
    {
        int idx = selectedIndices[0];
        uint8_t prio = m_models.at(idx).priority;

        switch (prio)
        {
        case PRIORITY_DO_NOT_DOWNLOAD:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_SKIP, MF_BYCOMMAND | MF_CHECKED);
            break;
        case PRIORITY_NORMAL:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_NORMAL, MF_BYCOMMAND | MF_CHECKED);
            break;
        case PRIORITY_HIGH:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_HIGH, MF_BYCOMMAND | MF_CHECKED);
            break;
        case PRIORITY_MAXIMUM:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_MAX, MF_BYCOMMAND | MF_CHECKED);
            break;
        }
    }

    int res = TrackPopupMenu(
        menu,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        0,
        m_hWnd,
        NULL);

    if (res == 0)
    {
        return;
    }

    Commands::PrioritizeFilesCommand prio;
    prio.indices = selectedIndices;

    switch (res)
    {
    case TORRENT_FILE_PRIO_SKIP:
        prio.priority = PRIORITY_DO_NOT_DOWNLOAD;
        break;
    case TORRENT_FILE_PRIO_NORMAL:
        prio.priority = PRIORITY_NORMAL;
        break;
    case TORRENT_FILE_PRIO_HIGH:
        prio.priority = PRIORITY_HIGH;
        break;
    case TORRENT_FILE_PRIO_MAX:
        prio.priority = PRIORITY_MAXIMUM;
        break;
    }

    SendCommand(PT_PRIORITIZEFILES, reinterpret_cast<LPARAM>(&prio));

    auto idxLo = std::min_element(selectedIndices.begin(), selectedIndices.end());
    auto idxHi = std::max_element(selectedIndices.begin(), selectedIndices.end());

    RedrawItems(*idxLo, *idxHi);
}

void TorrentFileListView::ToggleItemState(const std::vector<int>& selectedIndices)
{
    int lastItem = selectedIndices.at(selectedIndices.size() - 1);

    Commands::PrioritizeFilesCommand prio;
    prio.indices = selectedIndices;
    prio.priority = m_models.at(lastItem).priority == PRIORITY_DO_NOT_DOWNLOAD
        ? PRIORITY_NORMAL
        : PRIORITY_DO_NOT_DOWNLOAD;

    SendCommand(PT_PRIORITIZEFILES, reinterpret_cast<LPARAM>(&prio));

    auto idxLo = std::min_element(selectedIndices.begin(), selectedIndices.end());
    auto idxHi = std::max_element(selectedIndices.begin(), selectedIndices.end());

    RedrawItems(*idxLo, *idxHi);
}

std::wstring TorrentFileListView::GetPriorityString(int priority)
{
    switch (priority)
    {
    case PRIORITY_DO_NOT_DOWNLOAD:
        return TRW("do_not_download");
    case PRIORITY_NORMAL:
        return TRW("normal");
    case PRIORITY_HIGH:
        return TRW("high");
    case PRIORITY_MAXIMUM:
        return TRW("maximum");
    default:
        return TRW("unknown");
    }
}
