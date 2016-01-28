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
    TCHAR col_text[1024];
    StringCchCopy(col_text, _ARRAYSIZE(col_text), text.c_str());

    LVCOLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.pszText = col_text;
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
}

void list_view::add_column(int id, const std::wstring &text, int width, list_view::col_type_t type)
{
    list_view_column col;
    col.id = id;
    col.index = get_column_count();
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

    columns_.push_back(col);

    ListView_InsertColumn(
        handle(),
        col.index,
        &col.get_native_column());
}

signal_connector<std::wstring, int>& list_view::on_display()
{
    return on_display_;
}

LRESULT list_view::subclass_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch(msg)
    {
    case WM_NOTIFY:
    {
        LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);
        
        switch (nmhdr->code)
        {
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* inf = reinterpret_cast<NMLVDISPINFO*>(nmhdr);
            printf("");
            break;
        }
        }
        break;
    }
    }

    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

void list_view::clear()
{
    SendMessage(handle(), LVM_DELETEALLITEMS, 0, 0);
}

int list_view::get_column_count()
{
    return Header_GetItemCount(header_);
}

int list_view::get_item_count()
{
    return ListView_GetItemCount(handle());
}

std::vector<int> list_view::get_selected_items()
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

void list_view::insert_item(int index, const std::wstring &text)
{
    TCHAR item_text[1024];
    StringCchCopy(item_text, _ARRAYSIZE(item_text), text.c_str());

    LVITEM lv;
    lv.mask = LVIF_TEXT;
    lv.iItem = index;
    lv.iSubItem = 0;
    lv.pszText = item_text;

    ListView_InsertItem(handle(), &lv);
}

void list_view::set_extended_style(int style)
{
    ListView_SetExtendedListViewStyle(handle(), style);
}

void list_view::set_item(int index, int sub_index, const std::wstring &text)
{
    TCHAR item_text[1024];
    StringCchCopy(item_text, _ARRAYSIZE(item_text), text.c_str());

    LVITEM lv;
    lv.mask = LVIF_TEXT;
    lv.iItem = index;
    lv.iSubItem = sub_index;
    lv.pszText = item_text;

    ListView_SetItem(handle(), &lv);
}
