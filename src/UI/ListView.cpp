#include "ListView.hpp"

#include <strsafe.h>

using UI::ListView;

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
}

ListView::~ListView()
{
}

void ListView::AddColumn(int columnId, const std::wstring& title, int size, ColumnType type)
{
    m_list.InsertColumn(columnId, title.c_str(), type == ColumnType::Text ? LVCFMT_LEFT : LVCFMT_RIGHT, size);
}

HWND ListView::GetHandle()
{
    return m_list;
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
                GetItemText git;
                git.column_id = 123;
                git.item_index = inf->item.iItem;

                lv->m_list.GetParent().SendMessage(PT_LV_GETITEMTEXT, NULL, reinterpret_cast<LPARAM>(&git));
                StringCchCopy(inf->item.pszText, inf->item.cchTextMax, git.text.c_str());
            }

            break;
        }
        }

        break;
    }
    }

    return DefSubclassProc(hWnd, msg, wParam, lParam);
}
