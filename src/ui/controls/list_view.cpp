#include <picotorrent/ui/controls/list_view.hpp>

#include <picotorrent/ui/scaler.hpp>
#include <strsafe.h>
#include <vector>

using picotorrent::common::signals::signal;
using picotorrent::common::signals::signal_connector;
using picotorrent::ui::scaler;
using picotorrent::ui::controls::list_view;

LV_COLUMN list_view::list_view_column::get_native_column()
{
    LVCOLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.pszText = &text[0];
    col.cx = scaler::x(width);
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

signal_connector<std::wstring, const std::pair<int, int>&>& list_view::on_display()
{
    return on_display_;
}

signal_connector<void, const std::vector<int>&>& list_view::on_item_context_menu()
{
    return on_item_context_;
}

signal_connector<float, const std::pair<int, int>&>& list_view::on_progress()
{
    return on_progress_;
}

void list_view::refresh()
{
    int idx = ListView_GetTopIndex(handle());
    int bottom = ListView_GetItemCount(handle());

    ListView_RedrawItems(handle(), idx, bottom);
    ::UpdateWindow(handle());
}

void list_view::set_item_count(int count)
{
    ListView_SetItemCount(handle(), count);
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
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* inf = reinterpret_cast<NMLVDISPINFO*>(nmhdr);

            // Find column with index
            const list_view_column &col = lv->columns_.at(inf->item.iSubItem);

            std::pair<int, int> p = std::make_pair(col.id, inf->item.iItem);
            std::wstring text = lv->on_display_.emit(p)[0];

            StringCchCopy(inf->item.pszText, inf->item.cchTextMax, text.c_str());
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
                DrawText(hDc, progress_str, -1, &text, DT_CENTER | DT_VCENTER);

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
