#include <picotorrent/ui/torrent_list_view.hpp>

#include <picotorrent/ui/torrent_list_item.hpp>
#include <algorithm>
#include <strsafe.h>

using picotorrent::ui::torrent_list_item;
using picotorrent::ui::torrent_list_view;

torrent_list_view::torrent_list_view(HWND hParent)
    : hWnd_(NULL),
    hWnd_parent_(hParent),
    hTheme_progress_(NULL)
{
}

torrent_list_view::~torrent_list_view()
{
    CloseThemeData(hTheme_progress_);
}

void torrent_list_view::add_column(LPCTSTR name, int width, int format)
{
    HWND header = ListView_GetHeader(hWnd_);
    int count = Header_GetItemCount(header);

    TCHAR text[1024];
    StringCchCopy(text, _ARRAYSIZE(text), name);

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

void torrent_list_view::create()
{
    RECT rcClient;
    GetClientRect(hWnd_parent_, &rcClient);

    hWnd_ = CreateWindowEx(
        0,
        WC_LISTVIEW,
        0,
        WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT | LVS_SINGLESEL,
        0,
        0,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hWnd_parent_,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    if (hWnd_ == NULL)
    {
        DWORD err = GetLastError();
        throw new std::runtime_error(std::to_string(err));
    }

    ListView_SetExtendedListViewStyle(hWnd_, LVS_EX_FULLROWSELECT);

    hWnd_progress_ = CreateWindowEx(
        0,
        PROGRESS_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hWnd_,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    hTheme_progress_ = OpenThemeData(hWnd_progress_, L"PROGRESS");
}

int torrent_list_view::get_column_count()
{
    HWND header = ListView_GetHeader(hWnd_);
    return Header_GetItemCount(header);
}

torrent_list_view::sort_order torrent_list_view::get_column_sort(int colIndex)
{
    HWND hHeader = ListView_GetHeader(hWnd_);
    HDITEM hdrItem = { 0 };

    if (!hHeader)
    {
        return torrent_list_view::sort_order::none;
    }

    hdrItem.mask = HDI_FORMAT;

    if (Header_GetItem(hHeader, colIndex, &hdrItem))
    {
        if (hdrItem.fmt & HDF_SORTDOWN)
        {
            return torrent_list_view::sort_order::desc;
        }
        else if (hdrItem.fmt & HDF_SORTUP)
        {
            return torrent_list_view::sort_order::asc;
        }
    }

    return torrent_list_view::sort_order::none;
}

HWND torrent_list_view::handle()
{
    return hWnd_;
}

LRESULT torrent_list_view::on_custom_draw(LPNMLVCUSTOMDRAW lpCustomDraw, const torrent_list_item& item)
{
    switch (lpCustomDraw->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        return CDRF_NOTIFYITEMDRAW;
    case CDDS_ITEMPREPAINT:
        return CDRF_NOTIFYSUBITEMDRAW;
    case (CDDS_ITEMPREPAINT | CDDS_SUBITEM):
    {
        if(lpCustomDraw->iSubItem != COL_PROGRESS)
        {
            break;
        }

        HDC hDc = lpCustomDraw->nmcd.hdc;
        RECT rc = { 0 };
        ListView_GetSubItemRect(
            hWnd_,
            lpCustomDraw->nmcd.dwItemSpec,
            lpCustomDraw->iSubItem,
            LVIR_BOUNDS,
            &rc);

        // Paint the background
        rc.bottom -= 2;
        rc.left += 2;
        rc.right -= 2;
        rc.top += 2;
        DrawThemeBackground(
            hTheme_progress_,
            hDc,
            11, // TODO(put in #define)
            1, // TODO(put in #define)
            &rc,
            NULL);

        // Paint the filler
        rc.bottom -= 1;
        rc.right -= 1;
        rc.left += 1;
        rc.top += 1;

        int width = rc.right - rc.left;
        int newWidth = width * item.progress();
        rc.right = rc.left + newWidth;

        DrawThemeBackground(
            hTheme_progress_,
            hDc,
            5, // TODO(put in #define)
            1, // TODO(put in #define)
            &rc,
            NULL);

        RECT text = { 0 };
        ListView_GetSubItemRect(
            hWnd_,
            lpCustomDraw->nmcd.dwItemSpec,
            lpCustomDraw->iSubItem,
            LVIR_BOUNDS,
            &text);

        DrawText(hDc, item.progress_str().c_str(), -1, &text, DT_CENTER);

        return CDRF_SKIPDEFAULT;
    }
    }

    return FALSE;
}

void torrent_list_view::on_getdispinfo(NMLVDISPINFO* inf, const torrent_list_item &item)
{
    if (inf->item.mask & LVIF_TEXT)
    {
        switch (inf->item.iSubItem)
        {
        case COL_NAME:
        {
            StringCchCopy(inf->item.pszText, inf->item.cchTextMax, item.name().c_str());
            break;
        }
        case COL_QUEUE_POSITION:
        {
            StringCchCopy(inf->item.pszText, inf->item.cchTextMax, item.queue_position_str().c_str());
            break;
        }
        case COL_SIZE:
        {
            StringCchCopy(inf->item.pszText, inf->item.cchTextMax, item.size_str().c_str());
            break;
        }
        case COL_STATE:
        {
            StringCchCopy(inf->item.pszText, inf->item.cchTextMax, item.state_str().c_str());
            break;
        }
        case COL_DOWNLOAD_RATE:
        case COL_UPLOAD_RATE:
        {
            StringCchCopy(inf->item.pszText,
                inf->item.cchTextMax,
                inf->item.iSubItem == COL_DOWNLOAD_RATE
                ? item.download_rate_str().c_str()
                : item.upload_rate_str().c_str());
            break;
        }
        }
    }
}

void torrent_list_view::refresh()
{
    int idx = ListView_GetTopIndex(hWnd_);
    int bottom = (std::min)(ListView_GetItemCount(hWnd_), 100);

    ListView_RedrawItems(hWnd_, idx, bottom);
    ::UpdateWindow(hWnd_);
}

void torrent_list_view::resize(int width, int height)
{
    ::SetWindowPos(
        hWnd_,
        0,
        0,
        0,
        width,
        height,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void torrent_list_view::set_column_sort(int colIndex, torrent_list_view::sort_order order)
{
    HWND hHeader = ListView_GetHeader(hWnd_);
    HDITEM hdrItem = { 0 };

    if (!hHeader)
    {
        return;
    }

    hdrItem.mask = HDI_FORMAT;

    if (Header_GetItem(hHeader, colIndex, &hdrItem))
    {
        switch (order)
        {
        case torrent_list_view::sort_order::asc:
        {
            hdrItem.fmt = (hdrItem.fmt & ~HDF_SORTDOWN) | HDF_SORTUP;
            break;
        }
        case torrent_list_view::sort_order::desc:
        {
            hdrItem.fmt = (hdrItem.fmt & ~HDF_SORTUP) | HDF_SORTDOWN;
            break;
        }
        default:
        {
            hdrItem.fmt = hdrItem.fmt & ~(HDF_SORTDOWN | HDF_SORTUP);
            break;
        }
        }

        Header_SetItem(hHeader, colIndex, &hdrItem);
    }
}

void torrent_list_view::set_item_count(int count)
{
    ListView_SetItemCountEx(hWnd_, count, LVSICF_NOINVALIDATEALL);
}
