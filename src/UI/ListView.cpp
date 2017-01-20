#include "ListView.hpp"

#include "../resources.h"
#include "../Scaler.hpp"
#include "../UIState.hpp"

#include <strsafe.h>
#include <uxtheme.h>

#include <algorithm>

#define LV_SUBCLASS_ID 1890

using UI::ListView;

struct ListView::Column
{
    int id;
    int order;
    int position;
    std::wstring text;
    ColumnType type;
    bool visible;
    int width;
};

ListView::ListView(HWND hWndList)
    : CListViewCtrl(hWndList)
{
    SetExtendedListViewStyle(
        LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_HEADERDRAGDROP);

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
    c.text = title;
    c.type = type;
    c.visible = true;
    c.width = size;

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

void ListView::SelectAll()
{
    SetItemState(-1, LVIS_SELECTED, 2);
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

void ListView::LoadState(const std::string& key)
{
    UIState::ColumnStateMap m = UIState::GetInstance().GetListViewColumnState(key);
    if (m.empty()) { return; }

    // Order array
    int* pOrder = new int[m.size()];

    for (int i = 0; i < (int)m_cols.size(); i++)
    {
        Column& col = m_cols.at(i);
        if (m.find(col.id) == m.end()) { continue; }

        UIState::ColumnState& state = m.at(col.id);

        col.order = state.order;
        col.visible = state.visible;
        col.width = state.width;

        pOrder[i] = state.order;
        SetColumnWidth(i, col.visible ? col.width : 0);
    }

    SetColumnOrderArray((int)m.size(), pOrder);
    delete[] pOrder;
}

void ListView::SaveState(const std::string& key)
{
    UIState::ColumnStateMap m;

    for (Column& col : m_cols)
    {
        UIState::ColumnState s;
        s.order = col.order;
        s.visible = col.visible;
        s.width = col.width;

        m.insert({ col.id, s });
    }

    UIState::GetInstance().SetListViewColumnState(key, m);
}

bool ListView::IsPointInHeader(POINT p)
{
    CHeaderCtrl header = GetHeader();
    header.ScreenToClient(&p);

    for (int i = 0; i < header.GetItemCount(); i++)
    {
        RECT colRect;
        header.GetItemRect(i, &colRect);

        if (PtInRect(&colRect, p))
        {
            return true;
        }
    }

    return false;
}

void ListView::ShowColumnContextMenu(POINT p)
{
    HMENU hColumnSelector = CreatePopupMenu();

    for (int i = 0; i < (int)m_cols.size(); i++)
    {
        Column& col = m_cols.at(i);

        UINT uFlags = MF_STRING;

        if (col.visible)
        {
            uFlags |= MF_CHECKED;
        }

        AppendMenu(hColumnSelector, uFlags, 1000 + i, col.text.c_str());
    }

    int res = TrackPopupMenu(
        hColumnSelector,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        0,
        m_hWnd,
        NULL);

    if (res <= 0)
    {
        return;
    }

    res -= 1000;

    Column& col = m_cols.at(res);
    SetColumnWidth(res, col.visible ? 0 : col.width);
    col.visible = !col.visible;
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

        if (lv->IsPointInHeader(p))
        {
            lv->ShowColumnContextMenu(p);
            break;
        }

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

        switch (nmhdr->code)
        {
        case HDN_ENDDRAG:
        {
            LPNMHEADER lpHeader = reinterpret_cast<LPNMHEADER>(nmhdr);
            auto col = std::find_if(
                lv->m_cols.begin(),
                lv->m_cols.end(),
                [lpHeader](const Column& c) { return c.position == lpHeader->iItem; });
            if (col == lv->m_cols.end()) { break; }

            col->order = lpHeader->pitem->iOrder;
            break;
        }
        case HDN_DIVIDERDBLCLICK:
        {
            LPNMHEADER lpHeader = reinterpret_cast<LPNMHEADER>(nmhdr);
            auto col = std::find_if(
                lv->m_cols.begin(),
                lv->m_cols.end(),
                [lpHeader](const Column& c) { return c.position == lpHeader->iItem; });
            if (col == lv->m_cols.end()) { break; }

            col->width = lv->GetColumnWidth(lpHeader->iItem);
            break;
        }
        case HDN_ENDTRACK:
        {
            LPNMHEADER lpHeader = reinterpret_cast<LPNMHEADER>(nmhdr);
            auto col = std::find_if(
                lv->m_cols.begin(),
                lv->m_cols.end(),
                [lpHeader](const Column& c) { return c.position == lpHeader->iItem; });
            if (col == lv->m_cols.end()) { break; }

            col->width = lpHeader->pitem->cxy;
            break;
        }
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
        case NM_CLICK:
        {
            LPNMITEMACTIVATE lpItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(nmhdr);

            UINT flags = 0;
            int item = lv->HitTest(lpItemActivate->ptAction, &flags);

            if ((flags & LVHT_ONITEM) != LVHT_ONITEM
                && (flags & LVHT_ONITEMSTATEICON) == LVHT_ONITEMSTATEICON)
            {
                std::vector<int> selectedIndices{ lpItemActivate->iItem };

                int pos = lv->GetNextItem(-1, LVNI_SELECTED);
                while (pos != -1)
                {
                    if (pos != lpItemActivate->iItem)
                    {
                        selectedIndices.push_back(pos);
                    }
                    pos = lv->GetNextItem(pos, LVNI_SELECTED);
                }

                lv->ToggleItemState(selectedIndices);
            }

            break;
        }
        case NM_DBLCLK:
        {
            LPNMITEMACTIVATE lpnmia = reinterpret_cast<LPNMITEMACTIVATE>(lParam);
            std::vector<int> indices{ lpnmia->iItem };
            lv->OnItemActivated(indices);
            break;
        }
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* inf = reinterpret_cast<NMLVDISPINFO*>(nmhdr);
            
            int position = inf->item.iSubItem;
            auto col = std::find_if(
                lv->m_cols.begin(),
                lv->m_cols.end(),
                [position](const Column& c) { return c.position == position; });

            if (col == lv->m_cols.end())
            {
                break;
            }

            if (inf->item.mask & LVIF_TEXT)
            {
                if (col->type == ColumnType::Text || col->type == ColumnType::Number)
                {
                    std::wstring value = lv->GetItemText(col->id, inf->item.iItem);
                    StringCchCopy(inf->item.pszText, inf->item.cchTextMax, value.c_str());
                }
            }

            if (inf->item.mask & LVIF_IMAGE)
            {
                int idx = lv->GetItemIconIndex(inf->item.iItem);
                if (idx >= 0)
                {
                    inf->item.iImage = idx;
                }
            }

            if (inf->item.mask & LVIF_STATE)
            {
                inf->item.stateMask = LVIS_STATEIMAGEMASK;

                if (lv->GetItemIsChecked(inf->item.iItem))
                {
                    inf->item.state = INDEXTOSTATEIMAGEMASK(2);
                }
                else
                {
                    inf->item.state = INDEXTOSTATEIMAGEMASK(1);
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

                if (col == lv->m_cols.end()) { break; }
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
