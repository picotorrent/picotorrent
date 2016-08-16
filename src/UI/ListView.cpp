#include "ListView.hpp"

#include "../resources.h"
#include "../Scaler.hpp"

#include <strsafe.h>
#include <uxtheme.h>

#include <algorithm>

using UI::ListView;

struct ListView::Column
{
    int id;
    int position;
    ColumnType type;
};

ListView::ListView(HWND hWndList)
    : m_list(hWndList)
{
    ListView_SetExtendedListViewStyle(
        m_list,
        LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    SetWindowSubclass(
        m_list.GetParent(),
        &ListView::SubclassProc,
        123,
        (DWORD_PTR)this);

    // Load progress theme
    m_progress = CreateWindowEx(
        0,
        PROGRESS_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        m_list,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    m_progressTheme = OpenThemeData(
        m_progress,
        L"PROGRESS");
}

ListView::~ListView()
{
    CloseThemeData(m_progressTheme);
}

void ListView::AddColumn(int columnId, const std::wstring& title, int size, ColumnType type)
{
    int existingColumns = m_list.GetHeader().GetItemCount();
    int columnPosition = m_list.InsertColumn(
        existingColumns,
        title.c_str(),
        type == ColumnType::Text ? LVCFMT_LEFT : LVCFMT_RIGHT,
        size);

    Column c;
    c.id = columnId;
    c.position = columnPosition;
    c.type = type;

    m_cols.push_back(c);
}

HWND ListView::GetHandle()
{
    return m_list;
}

std::pair<int, int> ListView::GetVisibleIndices()
{
    std::pair<int, int> indices;
    indices.first = m_list.GetTopIndex();

    if (m_list.GetItemCount() == 0)
    {
        return std::pair<int, int>();
    }

    RECT rc;
    m_list.GetItemRect(indices.first, &rc, LVIR_BOUNDS);

    RECT client;
    m_list.GetClientRect(&client);

    int clientHeight = client.bottom - client.top;
    int itemHeight = rc.bottom - rc.top;

    indices.second = (clientHeight / itemHeight) + indices.first + 1;
    if (indices.second > m_list.GetItemCount()) { indices.second = m_list.GetItemCount(); }

    return indices;
}

void ListView::RedrawItems(int first, int last)
{
    m_list.RedrawItems(first, last);
}

void ListView::SetItemCount(int count)
{
    m_list.SetItemCount(count);
}

LRESULT ListView::SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    ListView *lv = reinterpret_cast<ListView*>(dwRefData);

    switch (msg)
    {
    case WM_CONTEXTMENU:
    {
        HWND hWndTarget = reinterpret_cast<HWND>(wParam);
        if (hWndTarget != lv->m_list) { break; }

        ShowContextMenu scm;
        scm.point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        int pos = lv->m_list.GetNextItem(-1, LVNI_SELECTED);
        while (pos != -1)
        {
            scm.selected_indices.push_back(pos);
            pos = lv->m_list.GetNextItem(pos, LVNI_SELECTED);
        }

        lv->m_list.GetParent().SendMessage(PT_LV_SHOWCONTEXTMENU, NULL, reinterpret_cast<LPARAM>(&scm));
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);
        if (nmhdr->hwndFrom != lv->m_list) { break; }

        switch (nmhdr->code)
        {
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* inf = reinterpret_cast<NMLVDISPINFO*>(nmhdr);
            
            if (inf->item.mask & LVIF_TEXT)
            {
                int position = inf->item.iSubItem;
                auto col = std::find_if(
                    lv->m_cols.begin(),
                    lv->m_cols.end(),
                    [position](const Column& c) { return c.position == position; });

                if (col != lv->m_cols.end()
                    && (col->type == ColumnType::Text || col->type == ColumnType::Number))
                {
                    GetItemText git;
                    git.column_id = col->id;
                    git.item_index = inf->item.iItem;

                    lv->m_list.GetParent().SendMessage(PT_LV_GETITEMTEXT, NULL, reinterpret_cast<LPARAM>(&git));
                    StringCchCopy(inf->item.pszText, inf->item.cchTextMax, git.text.c_str());
                }
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
                int position = lpCustomDraw->iSubItem;
                auto col = std::find_if(
                    lv->m_cols.begin(),
                    lv->m_cols.end(),
                    [position](const Column& c) { return c.position == position; });

                if (col->type != ColumnType::Progress) { break; }

                GetItemProgress gip;
                gip.column_id = col->id;
                gip.item_index = (int)lpCustomDraw->nmcd.dwItemSpec;
                gip.progress = -1;

                lv->m_list.GetParent().SendMessage(PT_LV_GETITEMPROGRESS, NULL, reinterpret_cast<LPARAM>(&gip));
                if (gip.progress < 0) { break; }

                HDC hDc = lpCustomDraw->nmcd.hdc;
                RECT rc = { 0 };
                lv->m_list.GetSubItemRect(
                    (int)lpCustomDraw->nmcd.dwItemSpec,
                    lpCustomDraw->iSubItem,
                    LVIR_BOUNDS,
                    &rc);

                // Paint the background
                rc.bottom -= SY(2);
                rc.left += SX(2);
                rc.right -= SX(2);
                rc.top += SY(2);
                DrawThemeBackground(
                    lv->m_progressTheme,
                    hDc,
                    11, // TODO(put in #define)
                    1, // TODO(put in #define)
                    &rc,
                    NULL);

                // Paint the filler
                rc.bottom -= SY(1);
                rc.right -= SX(1);
                rc.left += SX(1);
                rc.top += SY(1);

                int width = rc.right - rc.left;
                int newWidth = (int)(width * gip.progress);
                rc.right = rc.left + newWidth;

                DrawThemeBackground(
                    lv->m_progressTheme,
                    hDc,
                    5, // TODO(put in #define)
                    1, // TODO(put in #define)
                    &rc,
                    NULL);

                RECT text = { 0 };
                lv->m_list.GetSubItemRect(
                    (int)lpCustomDraw->nmcd.dwItemSpec,
                    lpCustomDraw->iSubItem,
                    LVIR_BOUNDS,
                    &text);

                TCHAR progress_str[100];
                StringCchPrintf(progress_str, ARRAYSIZE(progress_str), TEXT("%.2f%%"), gip.progress * 100);
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
