#include "torrentlistview.h"

#include <libtorrent/sha1_hash.hpp>
#include <stdexcept>
#include <string>

namespace lt = libtorrent;
using pico::TorrentListView;

TorrentListView::TorrentListView(HWND hWndParent)
    : hWnd_(NULL),
    hWndParent_(hWndParent)
{
}

void TorrentListView::AddColumn(LPTSTR text, int width, int format)
{
    HWND header = ListView_GetHeader(hWnd_);
    int count = Header_GetItemCount(header);

    LVCOLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.iSubItem = count;
    col.pszText = text;
    col.cx = width;
    col.fmt = format;

    if (ListView_InsertColumn(hWnd_, count, &col) == -1)
    {
        DWORD err = GetLastError();
        throw new std::runtime_error(std::to_string(err));
    }
}

void TorrentListView::Create()
{
    RECT rcClient;
    GetClientRect(hWndParent_, &rcClient);

    hWnd_ = CreateWindowEx(
        0,
        WC_LISTVIEW,
        0,
        WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT | LVS_SINGLESEL,
        0,
        0,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hWndParent_,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    if (hWnd_ == NULL)
    {
        DWORD err = GetLastError();
        throw new std::runtime_error(std::to_string(err));
    }

    ListView_SetExtendedListViewStyle(hWnd_, LVS_EX_FULLROWSELECT);
}

HWND TorrentListView::GetWindowHandle()
{
    return hWnd_;
}

void TorrentListView::Resize(int width, int height)
{
    SetWindowPos(hWnd_,
        0,
        0,
        0,
        width,
        height,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void TorrentListView::SetItemCount(int count)
{
    ListView_SetItemCountEx(hWnd_, count, LVSICF_NOINVALIDATEALL);
}

void TorrentListView::Refresh()
{
    int idx = ListView_GetTopIndex(hWnd_);
    int bottom = std::min(ListView_GetItemCount(hWnd_), 100);

    ListView_RedrawItems(hWnd_, idx, bottom);
    ::UpdateWindow(hWnd_);
}
