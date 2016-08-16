#pragma once

#include "../stdafx.h"

#include <string>

namespace UI
{
class ListView
{
public:
    enum ColumnType
    {
        Number,
        Progress,
        Text
    };

    struct GetItemText
    {
        int column_id;
        int item_index;
        std::wstring text;
    };

    ListView(HWND hWndList);
    ~ListView();

    void AddColumn(int columnId, const std::wstring& title, int size, ColumnType type);
    HWND GetHandle();
    void SetItemCount(int count);

private:
    static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    CListViewCtrl m_list;
};
}
