#include <picotorrent/ui/controls/list_view.hpp>

#include <commctrl.h>
#include <picotorrent/ui/scaler.hpp>
#include <strsafe.h>
#include <vector>

using picotorrent::ui::scaler;
using picotorrent::ui::controls::list_view;

list_view::list_view(HWND handle)
    : control_base(handle),
    header_(NULL)
{
    header_ = ListView_GetHeader(handle);
}

void list_view::add_column(const std::wstring &text, int width, int format)
{
    TCHAR col_text[1024];
    StringCchCopy(col_text, _ARRAYSIZE(col_text), text.c_str());

    LVCOLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.pszText = col_text;
    col.cx = scaler::x(width);
    col.fmt = format;

    if (ListView_InsertColumn(handle(), get_column_count(), &col) == -1)
    {
        DWORD err = GetLastError();
        throw new std::runtime_error(std::to_string(err));
    }
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
