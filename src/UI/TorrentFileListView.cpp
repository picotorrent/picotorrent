#include "TorrentFileListView.hpp"

#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"

#define LV_COL_NAME 0
#define LV_COL_SIZE 1
#define LV_COL_PRIO 2

using UI::TorrentFileListView;

TorrentFileListView::TorrentFileListView(HWND hWnd)
    : ListView::ListView(hWnd)
{
    AddColumn(LV_COL_NAME, TRW("name"), SX(270), ColumnType::Text);
    AddColumn(LV_COL_SIZE, TRW("size"), SX(80), ColumnType::Number);
    AddColumn(LV_COL_PRIO, TRW("priority"), SX(120), ColumnType::Text);
}

void TorrentFileListView::UpdateModel(std::unique_ptr<TorrentFileListView::ViewModel> vm)
{
    m_model = std::move(vm);
}

std::wstring TorrentFileListView::GetItemText(int columnId, int itemIndex)
{
    switch (columnId)
    {
    case LV_COL_NAME:
        return m_model->GetFileName(itemIndex);
    case LV_COL_SIZE:
        TCHAR s[1024];
        StrFormatByteSize64(m_model->GetFileSize(itemIndex), s, ARRAYSIZE(s));
        return s;
    case LV_COL_PRIO:
        return GetPriorityString(m_model->GetFilePriority(itemIndex));
    }

    return L"?unknown column?";
}

void TorrentFileListView::ShowContextMenu(POINT p, const std::vector<int>& selectedIndices)
{
    HMENU prioMenu = CreateMenu();
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_MAX, TRW("maximum"));
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_HIGH, TRW("high"));
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_NORMAL, TRW("normal"));
    AppendMenu(prioMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_SKIP, TRW("do_not_download"));

    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_POPUP, (UINT_PTR)prioMenu, TRW("priority"));

    // If only one file is selected, check that files priority
    if (selectedIndices.size() == 1)
    {
        int idx = selectedIndices[0];
        uint8_t prio = m_model->GetFilePriority(idx);

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
        GetHandle(),
        NULL);

    for (int idx : selectedIndices)
    {
        switch (res)
        {
        case TORRENT_FILE_PRIO_SKIP:
            m_model->SetFilePriority(idx, PRIORITY_DO_NOT_DOWNLOAD);
            break;
        case TORRENT_FILE_PRIO_NORMAL:
            m_model->SetFilePriority(idx, PRIORITY_NORMAL);
            break;
        case TORRENT_FILE_PRIO_HIGH:
            m_model->SetFilePriority(idx, PRIORITY_HIGH);
            break;
        case TORRENT_FILE_PRIO_MAX:
            m_model->SetFilePriority(idx, PRIORITY_MAXIMUM);
            break;
        }
    }

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
