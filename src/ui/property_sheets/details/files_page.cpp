#include <picotorrent/ui/property_sheets/details/files_page.hpp>

#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <commctrl.h>

using picotorrent::ui::property_sheets::details::files_page;
using picotorrent::ui::scaler;

files_page::files_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_FILES);
    set_title_id(IDS_DETAILS_FILES_TITLE);
}

BOOL files_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

void files_page::on_init()
{
    HWND hFilesList = GetDlgItem(handle(), ID_DETAILS_FILES_LIST);

    LVCOLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.pszText = L"Name";
    col.cx = scaler::x(200);
    col.fmt = LVCFMT_LEFT;

    ListView_InsertColumn(hFilesList, 0, &col);

    col.pszText = L"Size";
    col.cx = scaler::x(80);
    col.fmt = LVCFMT_RIGHT;

    ListView_InsertColumn(hFilesList, 1, &col);

    col.pszText = L"Progress";
    col.cx = scaler::x(100);
    col.fmt = LVCFMT_LEFT;

    ListView_InsertColumn(hFilesList, 2, &col);
}
