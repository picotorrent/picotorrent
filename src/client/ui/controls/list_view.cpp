#include <picotorrent/client/ui/controls/list_view.hpp>

#include <picotorrent/client/ui/scaler.hpp>
#include <strsafe.h>
#include <vector>

using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;
using picotorrent::client::ui::scaler;
using picotorrent::client::ui::controls::list_view;

LV_COLUMN list_view::list_view_column::get_native_column()
{
    LVCOLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.pszText = &text[0];
    col.cx = width;
    col.fmt = format;

    return col;
}

list_view::list_view(HWND handle)
    : control_base(handle),
    header_(NULL)
{
    header_ = ListView_GetHeader(handle);

    // Set extended style.
    ListView_SetExtendedListViewStyle(handle, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    // Create progress bar and theme
    progress_ = CreateWindowEx(
        0,
        PROGRESS_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        handle,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    progress_theme_ = OpenThemeData(progress_, L"PROGRESS");

    // Subclass parent WndProc
    SetWindowSubclass(
        GetParent(handle),
        &list_view::subclass_proc,
        123,
        (DWORD_PTR)this);
}

list_view::~list_view()
{
    CloseThemeData(progress_theme_);
}

void list_view::add_column(int id, const std::wstring &text, int width, list_view::col_type_t type)
{
    list_view_column col;
    col.id = id;
    col.text = text;
    col.width = width;
    col.type = type;

    if (type == col_type_t::number)
    {
        col.format = LVCFMT_RIGHT;
    }
    else
    {
        col.format = LVCFMT_LEFT;
    }

    int index = Header_GetItemCount(header_);
    columns_.insert({ index, col });

    LVCOLUMN nvc = col.get_native_column();
    ListView_InsertColumn(
        handle(),
        index,
        &nvc);
}

std::vector<int> list_view::get_selection()
{
    std::vector<int> items;
    int item = ListView_GetNextItem(handle(), -1, LVNI_SELECTED);

    while (item > -1)
    {
        items.push_back(item);
        item = ListView_GetNextItem(handle(), item, LVNI_SELECTED);
    }

    return items;
}

list_view::sort_order_t list_view::get_sort_order(int columnId)
{
    HWND hHeader = ListView_GetHeader(handle());

    if (!hHeader)
    {
        return sort_order_t::unknown;
    }

    // Find column index by id
    auto it = std::find_if(
        columns_.begin(),
        columns_.end(),
        [columnId](const std::pair<int, list_view_column> &p) { return p.second.id == columnId; });

    if (it == columns_.end())
    {
        return sort_order_t::unknown;
    }

    HDITEM hdrItem = { 0 };
    hdrItem.mask = HDI_FORMAT;

    if (Header_GetItem(hHeader, it->first, &hdrItem))
    {
        if (hdrItem.fmt & HDF_SORTDOWN) { return sort_order_t::desc; }
        if (hdrItem.fmt & HDF_SORTUP) { return sort_order_t::asc; }
    }

    return sort_order_t::unknown;
}

signal_connector<std::wstring, const std::pair<int, int>&>& list_view::on_display()
{
    return on_display_;
}

signal_connector<void, const std::vector<int>&>& list_view::on_item_context_menu()
{
    return on_item_context_;
}

signal_connector<int, const std::pair<int, int>&>& list_view::on_item_image()
{
    return on_item_image_;
}

signal_connector<float, const std::pair<int, int>&>& list_view::on_progress()
{
    return on_progress_;
}

signal_connector<void, const std::pair<int, list_view::sort_order_t>&>& list_view::on_sort()

{
    return on_sort_;
}

void list_view::refresh()
{
    int idx = ListView_GetTopIndex(handle());
    int bottom = ListView_GetItemCount(handle());

    ListView_RedrawItems(handle(), idx, bottom);
    ::UpdateWindow(handle());
}

void list_view::set_image_list(HIMAGELIST img)
{
    ListView_SetImageList(handle(), img, LVSIL_SMALL);
}

void list_view::set_item_count(int count)
{
    ListView_SetItemCountEx(handle(), count, LVSICF_NOSCROLL);
}

void list_view::set_column_sort(int columnId, list_view::sort_order_t order)
{
    HWND hHeader = ListView_GetHeader(handle());
    HDITEM hdrItem = { 0 };

    if (!hHeader)
    {
        return;
    }

    // Find column index by id
    auto it = std::find_if(
        columns_.begin(),
        columns_.end(),
        [columnId](const std::pair<int, list_view_column> &p) { return p.second.id == columnId; });

    if (it == columns_.end())
    {
        return;
    }

    hdrItem.mask = HDI_FORMAT;

    if (Header_GetItem(hHeader, it->first, &hdrItem))
    {
        switch (order)
        {
        case sort_order_t::asc:
        {
            hdrItem.fmt = (hdrItem.fmt & ~HDF_SORTDOWN) | HDF_SORTUP;
            break;
        }
        case sort_order_t::desc:
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

        Header_SetItem(hHeader, it->first, &hdrItem);
    }
}

void list_view::resize(int width, int height)
{
    ::SetWindowPos(
        handle(),
        0,
        0,
        0,
        width,
        height,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

LRESULT list_view::subclass_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    list_view *lv = reinterpret_cast<list_view*>(dwRefData);

    switch(msg)
    {
    case WM_CONTEXTMENU:
    {
        HWND hTarget = reinterpret_cast<HWND>(wParam);
        if (hTarget != lv->handle()) { break; }

        std::vector<int> selection;
        int pos = ListView_GetNextItem(hTarget, -1, LVNI_SELECTED);

        while (pos != -1)
        {
            selection.push_back(pos);
            pos = ListView_GetNextItem(hTarget, pos, LVNI_SELECTED);
        }
        lv->on_item_context_.emit(selection);
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);

        if (nmhdr->hwndFrom != lv->handle())
        {
            break;
        }

        switch (nmhdr->code)
        {
        case LVN_COLUMNCLICK:
        {
            LPNMLISTVIEW lpListView = reinterpret_cast<LPNMLISTVIEW>(nmhdr);
            int columnIndex = lpListView->iSubItem;
            list_view_column &col = lv->columns_.at(columnIndex);
            sort_order_t currentSortOrder = lv->get_sort_order(col.id);
            sort_order_t newSortOrder = sort_order_t::asc;

            if (currentSortOrder == sort_order_t::asc) { newSortOrder = sort_order_t::desc; }
            if (currentSortOrder == sort_order_t::desc) { newSortOrder = sort_order_t::asc; }

            for (auto &it : lv->columns_)
            {
                lv->set_column_sort(it.first, sort_order_t::unknown);
            }

            lv->on_sort_.emit({ col.id, newSortOrder });

            lv->refresh();
            lv->set_column_sort(col.id, newSortOrder);

            break;
        }
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* inf = reinterpret_cast<NMLVDISPINFO*>(nmhdr);

            // Find column with index
            const list_view_column &col = lv->columns_.at(inf->item.iSubItem);
            std::pair<int, int> p = std::make_pair(col.id, inf->item.iItem);

            if (inf->item.mask & LVIF_TEXT)
            {
                std::wstring text = lv->on_display_.emit(p)[0];
                StringCchCopy(inf->item.pszText, inf->item.cchTextMax, text.c_str());
            }

            if (inf->item.mask & LVIF_IMAGE)
            {
                std::vector<int> index = lv->on_item_image_.emit(p);
                if (index.size() <= 0) { break; }
                inf->item.iImage = index[0];
            }

            break;
        }
        case NM_CUSTOMDRAW:
        {
            LPNMLVCUSTOMDRAW lpCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(nmhdr);

            switch (lpCustomDraw->nmcd.dwDrawStage)
            {
            case CDDS_PREPAINT:
                return CDRF_NOTIFYITEMDRAW;
            case CDDS_ITEMPREPAINT:
                return CDRF_NOTIFYSUBITEMDRAW;
            case (CDDS_ITEMPREPAINT | CDDS_SUBITEM):
            {
                const list_view_column &col = lv->columns_.at(lpCustomDraw->iSubItem);
                if (col.type != list_view::progress) { break; }

                std::pair<int, int> p = std::make_pair(col.id, (int)lpCustomDraw->nmcd.dwItemSpec);
                float val = lv->on_progress_.emit(p)[0];

                HDC hDc = lpCustomDraw->nmcd.hdc;
                RECT rc = { 0 };
                ListView_GetSubItemRect(
                    lv->handle(),
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
                    lv->progress_theme_,
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
                int newWidth = (int)(width * val);
                rc.right = rc.left + newWidth;

                DrawThemeBackground(
                    lv->progress_theme_,
                    hDc,
                    5, // TODO(put in #define)
                    1, // TODO(put in #define)
                    &rc,
                    NULL);

                RECT text = { 0 };
                ListView_GetSubItemRect(
                    lv->handle(),
                    lpCustomDraw->nmcd.dwItemSpec,
                    lpCustomDraw->iSubItem,
                    LVIR_BOUNDS,
                    &text);

                TCHAR progress_str[100];
                StringCchPrintf(progress_str, ARRAYSIZE(progress_str), TEXT("%.2f%%"), val * 100);
                DrawText(hDc, progress_str, -1, &text, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

                return CDRF_SKIPDEFAULT;
            }
            }
        }
        }
        break;
    }
    }

    return DefSubclassProc(hWnd, msg, wParam, lParam);
}
