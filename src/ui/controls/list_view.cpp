#include <picotorrent/ui/controls/list_view.hpp>

#include <commctrl.h>
#include <picotorrent/ui/scaler.hpp>
#include <strsafe.h>

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
    col.iSubItem = get_column_count();
    col.pszText = col_text;
    col.cx = scaler::x(width);
    col.fmt = format;

    if (ListView_InsertColumn(handle(), get_column_count(), &col) == -1)
    {
        DWORD err = GetLastError();
        throw new std::runtime_error(std::to_string(err));
    }
}

int list_view::get_column_count()
{
    return Header_GetItemCount(header_);
}
