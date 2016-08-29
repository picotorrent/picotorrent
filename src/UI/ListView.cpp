#include "ListView.hpp"

#include "../resources.h"
#include "../Scaler.hpp"

#include <strsafe.h>
#include <uxtheme.h>

#include <algorithm>

#define LV_SUBCLASS_ID 1890

using UI::ListView;

struct ListView::Column
{
    int id;
    int position;
    ColumnType type;
};

ListView::ListView(HWND hWndList)
    : CListViewCtrl(hWndList)
{
    SetExtendedListViewStyle(
        LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    SetWindowSubclass(
        GetParent(),
        &ListView::SubclassProc,
        LV_SUBCLASS_ID,
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
        m_hWnd,
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
    int existingColumns = GetHeader().GetItemCount();
    int columnPosition = InsertColumn(
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

ListView::SortOrder ListView::GetSortOrder(int columnId)
{
    auto col = std::find_if(
        m_cols.begin(),
        m_cols.end(),
        [columnId](const Column& c) { return c.id == columnId; });

    if (col == m_cols.end()) { return SortOrder::Unknown; }

    HDITEM hdrItem = { 0 };
    hdrItem.mask = HDI_FORMAT;

    if (GetHeader().GetItem(col->position, &hdrItem))
    {
        if (hdrItem.fmt & HDF_SORTDOWN) { return SortOrder::Descending; }
        if (hdrItem.fmt & HDF_SORTUP) { return SortOrder::Ascending; }
    }

    return SortOrder::Unknown;
}

std::pair<int, int> ListView::GetVisibleIndices()
{
    std::pair<int, int> indices;
    indices.first = GetTopIndex();

    if (GetItemCount() == 0)
    {
        return std::pair<int, int>();
    }

    RECT rc;
    GetItemRect(indices.first, &rc, LVIR_BOUNDS);

    RECT client;
    GetClientRect(&client);

    int clientHeight = client.bottom - client.top;
    int itemHeight = rc.bottom - rc.top;

    indices.second = (clientHeight / itemHeight) + indices.first + 1;
    if (indices.second > GetItemCount()) { indices.second = GetItemCount(); }

    return indices;
}

void ListView::SendCommand(UINT uMsg, LPARAM lParam)
{
    GetParent().SendMessage(uMsg, NULL, lParam);
}

void ListView::SetSortOrder(int columnId, SortOrder order)
{
    auto col = std::find_if(
        m_cols.begin(),
        m_cols.end(),
        [columnId](const Column& c) { return c.id == columnId; });

    if (col == m_cols.end()) { return; }

    HDITEM hdrItem = { 0 };
    hdrItem.mask = HDI_FORMAT;

    if (!GetHeader().GetItem(col->position, &hdrItem))
    {
        return;
    }

    switch (order)
    {
    case SortOrder::Ascending:
    {
        hdrItem.fmt = (hdrItem.fmt & ~HDF_SORTDOWN) | HDF_SORTUP;
        break;
    }
    case SortOrder::Descending:
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

    GetHeader().SetItem(col->position, &hdrItem);
}

LRESULT ListView::SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    ListView *lv = reinterpret_cast<ListView*>(dwRefData);

    switch (msg)
    {
    case WM_CONTEXTMENU:
    {
        HWND hWndTarget = reinterpret_cast<HWND>(wParam);
        if (hWndTarget != lv->m_hWnd) { break; }

        POINT p = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        std::vector<int> selectedIndices;

        int pos = lv->GetNextItem(-1, LVNI_SELECTED);
        while (pos != -1)
        {
            selectedIndices.push_back(pos);
            pos = lv->GetNextItem(pos, LVNI_SELECTED);
        }

        lv->ShowContextMenu(p, selectedIndices);
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);
        if (nmhdr->hwndFrom != lv->m_hWnd) { break; }

        switch (nmhdr->code)
        {
        case LVN_COLUMNCLICK:
        {
            LPNMLISTVIEW lpListView = reinterpret_cast<LPNMLISTVIEW>(nmhdr);
            int position = lpListView->iSubItem;
            auto col = std::find_if(
                lv->m_cols.begin(),
                lv->m_cols.end(),
                [position](const Column& c) { return c.position == position; });

            if (col == lv->m_cols.end()) { break; }

            SortOrder currentSortOrder = lv->GetSortOrder(col->id);
            SortOrder newSortOrder = SortOrder::Ascending;
            if (currentSortOrder == SortOrder::Ascending) { newSortOrder = SortOrder::Descending; }
            if (currentSortOrder == SortOrder::Descending) { newSortOrder = SortOrder::Ascending; }

            if (!lv->Sort(col->id, newSortOrder))
            {
                break;
            }

            for (auto& it : lv->m_cols)
            {
                lv->SetSortOrder(it.id, SortOrder::Unknown);
            }

            auto idx = lv->GetVisibleIndices();
            lv->RedrawItems(idx.first, idx.second);
            lv->SetSortOrder(col->id, newSortOrder);

            break;
        }
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
                    std::wstring value = lv->GetItemText(col->id, inf->item.iItem);
                    StringCchCopy(inf->item.pszText, inf->item.cchTextMax, value.c_str());
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

                float progress = lv->GetItemProgress(col->id, (int)lpCustomDraw->nmcd.dwItemSpec);
                if (progress < 0) { break; }

                HDC hDc = lpCustomDraw->nmcd.hdc;
                RECT rc = { 0 };
                lv->GetSubItemRect(
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
                int newWidth = (int)(width * progress);
                rc.right = rc.left + newWidth;

                DrawThemeBackground(
                    lv->m_progressTheme,
                    hDc,
                    5, // TODO(put in #define)
                    1, // TODO(put in #define)
                    &rc,
                    NULL);

                RECT text = { 0 };
                lv->GetSubItemRect(
                    (int)lpCustomDraw->nmcd.dwItemSpec,
                    lpCustomDraw->iSubItem,
                    LVIR_BOUNDS,
                    &text);

                TCHAR progress_str[100];
                StringCchPrintf(progress_str, ARRAYSIZE(progress_str), TEXT("%.2f%%"), progress * 100);
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
