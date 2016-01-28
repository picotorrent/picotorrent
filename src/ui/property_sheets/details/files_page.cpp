#include <picotorrent/ui/property_sheets/details/files_page.hpp>

#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/scaler.hpp>

#include <shlwapi.h>
#include <strsafe.h>

using picotorrent::ui::property_sheets::details::files_page;
using picotorrent::ui::scaler;

files_page::files_page()
    : progress_theme_(NULL)
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_DETAILS_FILES);
    set_title_id(IDS_DETAILS_FILES_TITLE);
}

void files_page::add_file(const std::wstring &name, uint64_t size, float progress)
{
    HWND hFilesList = GetDlgItem(handle(), ID_DETAILS_FILES_LIST);

    TCHAR name_str[1024];
    StringCchCopy(name_str, ARRAYSIZE(name_str), name.c_str());

    LVITEM item = { 0 };
    item.pszText = name_str;
    item.mask = LVIF_TEXT;
    item.stateMask = 0;
    item.iSubItem = 0;
    item.state = 0;

    ListView_InsertItem(hFilesList, &item);

    TCHAR size_str[100];
    StrFormatByteSize64(
        size,
        size_str,
        ARRAYSIZE(size_str));

    item.pszText = size_str;
    item.iSubItem = 1;

    ListView_SetItem(hFilesList, &item);

    files_progress_.push_back(progress);
}

void files_page::refresh()
{
    HWND hFilesList = GetDlgItem(handle(), ID_DETAILS_FILES_LIST);

    int idx = ListView_GetTopIndex(hFilesList);
    int bottom = ListView_GetItemCount(hFilesList);

    ListView_RedrawItems(hFilesList, idx, bottom);
    ::UpdateWindow(hFilesList);
}

void files_page::update_file_progress(int index, float progress)
{
    files_progress_[index] = progress;
}

BOOL files_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

void files_page::on_init_dialog()
{
    HWND hFilesList = GetDlgItem(handle(), ID_DETAILS_FILES_LIST);

    // Set style (full row select)
    ListView_SetExtendedListViewStyle(hFilesList, LVS_EX_FULLROWSELECT);

    LVCOLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.pszText = L"Name";
    col.cx = scaler::x(180);
    col.fmt = LVCFMT_LEFT;

    ListView_InsertColumn(hFilesList, 0, &col);

    col.pszText = L"Size";
    col.cx = scaler::x(80);
    col.fmt = LVCFMT_RIGHT;

    ListView_InsertColumn(hFilesList, 1, &col);

    col.pszText = L"Progress";
    col.cx = scaler::x(80);
    col.fmt = LVCFMT_LEFT;

    ListView_InsertColumn(hFilesList, 2, &col);

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
        handle(),
        NULL,
        GetModuleHandle(NULL),
        NULL);

    progress_theme_ = OpenThemeData(progress_, L"PROGRESS");
}

bool files_page::on_notify(HWND hDlg, LPNMHDR nmhdr, LRESULT &res)
{
    switch (nmhdr->code)
    {
    case NM_CUSTOMDRAW:
        if (nmhdr->idFrom == ID_DETAILS_FILES_LIST)
        {
            handle_draw_progress(reinterpret_cast<LPNMLVCUSTOMDRAW>(nmhdr), res);
            return true;
        }
        break;
    }

    return false;
}

void files_page::handle_draw_progress(LPNMLVCUSTOMDRAW lpCustomDraw, LRESULT &lResult)
{
    lResult = FALSE;

    switch (lpCustomDraw->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        lResult = CDRF_NOTIFYITEMDRAW;
        break;
    case CDDS_ITEMPREPAINT:
        lResult = CDRF_NOTIFYSUBITEMDRAW;
        break;
    case (CDDS_ITEMPREPAINT | CDDS_SUBITEM):
    {
        if (lpCustomDraw->iSubItem != 2
            || files_progress_.empty())
        {
            break;
        }
        
        HWND hFilesList = GetDlgItem(handle(), ID_DETAILS_FILES_LIST);
        float file_progress = files_progress_[lpCustomDraw->nmcd.dwItemSpec];

        HDC hDc = lpCustomDraw->nmcd.hdc;
        RECT rc = { 0 };
        ListView_GetSubItemRect(
            hFilesList,
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
            progress_theme_,
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
        int newWidth = (int)(width * file_progress);
        rc.right = rc.left + newWidth;

        DrawThemeBackground(
            progress_theme_,
            hDc,
            5, // TODO(put in #define)
            1, // TODO(put in #define)
            &rc,
            NULL);

        RECT text = { 0 };
        ListView_GetSubItemRect(
            hFilesList,
            lpCustomDraw->nmcd.dwItemSpec,
            lpCustomDraw->iSubItem,
            LVIR_BOUNDS,
            &text);

        TCHAR p[100];
        StringCchPrintf(p, ARRAYSIZE(p), TEXT("%.2f%%"), file_progress * 100);
        DrawText(hDc, p, -1, &text, DT_CENTER | DT_VCENTER);

        lResult = CDRF_SKIPDEFAULT;
    }
    }
}
