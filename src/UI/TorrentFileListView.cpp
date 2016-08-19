#include "TorrentFileListView.hpp"

using UI::TorrentFileListView;

TorrentFileListView::TorrentFileListView(HWND hWnd,
    const libtorrent::file_storage& files,
    const std::vector<uint8_t>& prio)
    : ListView::ListView(hWnd),
    m_files(files),
    m_prio(prio)
{
}

std::wstring TorrentFileListView::GetItemText(int columnId, int itemIndex)
{
    return L"";
}
