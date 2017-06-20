#include "ListView.hpp"

#include "../resources.h"
#include "../Scaler.hpp"
#include "../UIState.hpp"

#include <strsafe.h>
#include <uxtheme.h>

#include <algorithm>

#define LV_SUBCLASS_ID 1890

using UI::ListView;

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

void ListView::AddColumn(std::wstring& text, int position, int width, int format, int mask, bool isProgressCol)
{
    LVCOLUMN lvc = { 0 };
    lvc.cx = width;
    lvc.fmt = format;
    lvc.mask = mask;
    lvc.pszText = &text[0];

    InsertColumn(position, &lvc);

    ColumnState state;
    state.showProgress = isProgressCol;
    state.visible = true;
    state.width = width;

    m_columns.insert({ position, state });
}

ListView::SortOrder ListView::GetSortOrder(int columnId)
{
    HDITEM hdrItem = { 0 };
    hdrItem.mask = HDI_FORMAT;

    if (GetHeader().GetItem(columnId, &hdrItem))
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
    HDITEM hdrItem = { 0 };
    hdrItem.mask = HDI_FORMAT;

    if (!GetHeader().GetItem(columnId, &hdrItem))
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

    GetHeader().SetItem(columnId, &hdrItem);
}

void ListView::LoadState(const std::string& key)
{
    UIState::ListViewState state = UIState::GetInstance().GetListViewState(key);

    if (state.order.size() != m_columns.size()
        && state.visibility.size() != m_columns.size()
        && state.width.size() != m_columns.size())
    {
        return;
    }

    int* order = new int[m_columns.size()];

    for each (auto pair in m_columns)
    {
        m_columns.at(pair.first).visible = state.visibility.at(pair.first);
        m_columns.at(pair.first).width = state.width.at(pair.first);

        order[pair.first] = state.order.at(pair.first);

        SetColumnWidth(
            pair.first,
            m_columns.at(pair.first).visible
            ? m_columns.at(pair.first).width
            : 0);
    }

    ListView_SetColumnOrderArray(*this, m_columns.size(), order);
    delete[] order;
}

void ListView::SaveState(const std::string& key)
{
    size_t cols = m_columns.size();

    UIState::ListViewState state;
    state.order.resize(cols);
    state.visibility.resize(cols);
    state.width.resize(cols);

    int* order = new int[cols];
    GetColumnOrderArray(static_cast<int>(cols), order);

    for each (auto pair in m_columns)
    {
        state.order.at(pair.first) = order[pair.first];
        state.visibility.at(pair.first) = pair.second.visible;
        state.width.at(pair.first) = pair.second.width;
    }

    delete[] order;

    UIState::GetInstance().SetListViewState(key, state);
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

    int cols = GetHeader().GetItemCount();
    int* colOrder = new int[cols];
    GetColumnOrderArray(cols, colOrder);

    HMENU hColumnMenu = CreatePopupMenu();

    for (int i = 0; i < cols; i++)
    {
        TCHAR txt[1024];

        HDITEM hdi = { 0 };
        hdi.mask = HDI_TEXT;
        hdi.pszText = txt;
        hdi.cchTextMax = ARRAYSIZE(txt);

        if (GetHeader().GetItem(colOrder[i], &hdi))
        {
            UINT uFlags = MF_STRING;
            if (m_columns.at(colOrder[i]).visible) { uFlags |= MF_CHECKED; }

            AppendMenu(hColumnMenu, uFlags, i + 1000, hdi.pszText);
        }
    }

    int res = TrackPopupMenu(
        hColumnMenu,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        NULL,
        *this,
        NULL);

    if (res <= 0)
    {
        return;
    }

    res -= 1000;

    ColumnState& cs = m_columns.at(colOrder[res]);

    SetColumnWidth(colOrder[res], cs.visible ? 0 : cs.width);
    cs.visible = !cs.visible;
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
    case WM_DESTROY:
    {
        lv->OnDestroy();
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);

        switch (nmhdr->code)
        {
        case HDN_DIVIDERDBLCLICK:
        {
            LPNMHEADER lpHeader = reinterpret_cast<LPNMHEADER>(nmhdr);
            lv->m_columns.at(lpHeader->iItem).width = lv->GetColumnWidth(lpHeader->iItem);
            break;
        }
        case HDN_ENDTRACK:
        {
            LPNMHEADER lpHeader = reinterpret_cast<LPNMHEADER>(nmhdr);
            lv->m_columns.at(lpHeader->iItem).width = lpHeader->pitem->cxy;
            break;
        }
        case LVN_COLUMNCLICK:
        {
            LPNMLISTVIEW lpListView = reinterpret_cast<LPNMLISTVIEW>(nmhdr);

            SortOrder currentSortOrder = lv->GetSortOrder(lpListView->iSubItem);
            SortOrder newSortOrder = SortOrder::Ascending;
            if (currentSortOrder == SortOrder::Ascending) { newSortOrder = SortOrder::Descending; }
            if (currentSortOrder == SortOrder::Descending) { newSortOrder = SortOrder::Ascending; }

            if (!lv->Sort(lpListView->iSubItem, newSortOrder))
            {
                break;
            }

            for (int i = 0; i < lv->GetHeader().GetItemCount(); i++)
            {
                lv->SetSortOrder(i, SortOrder::Unknown);
            }

            auto idx = lv->GetVisibleIndices();
            lv->RedrawItems(idx.first, idx.second);
            lv->SetSortOrder(lpListView->iSubItem, newSortOrder);

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

            UINT flags = 0;
            int item = lv->HitTest(lpnmia->ptAction, &flags);

            if (flags & LVHT_ONITEM)
            {
                std::vector<int> indices{ lpnmia->iItem };
                lv->OnItemActivated(indices);
            }

            break;
        }
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* inf = reinterpret_cast<NMLVDISPINFO*>(nmhdr);
            ColumnState& cs = lv->m_columns.at(inf->item.iSubItem);

            if (inf->item.mask & LVIF_TEXT)
            {
                if (!cs.showProgress)
                {
                    std::wstring value = lv->GetItemText(inf->item.iSubItem, inf->item.iItem);
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
                ColumnState& cs = lv->m_columns.at(lpCustomDraw->iSubItem);
                if (!cs.showProgress) { break; }

                float progress = lv->GetItemProgress(lpCustomDraw->iSubItem, (int)lpCustomDraw->nmcd.dwItemSpec);
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
