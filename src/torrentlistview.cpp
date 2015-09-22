#include "torrentlistview.h"

#include <stdexcept>
#include <string>

using pico::TorrentListView;

TorrentListView::TorrentListView(HWND hWndParent)
    : hWnd_(NULL),
    hWndParent_(hWndParent)
{
}

void TorrentListView::AddColumn(LPSTR text, int width, int format)
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
        LVS_EX_FULLROWSELECT,
        WC_LISTVIEW,
        0,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
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
