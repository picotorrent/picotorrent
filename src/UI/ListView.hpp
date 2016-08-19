#pragma once

#include "../stdafx.h"

#include <string>
#include <vector>

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

    enum SortOrder
    {
        Unknown,
        Ascending,
        Descending
    };

    struct GetItemText
    {
        int column_id;
        int item_index;
        std::wstring text;
    };

    struct GetItemProgress
    {
        int column_id;
        int item_index;
        float progress;
    };

    struct SetColumnSortOrder
    {
        int column_id;
        bool did_sort;
        SortOrder order;
    };

    struct ShowContextMenu
    {
        POINT point;
        std::vector<int> selected_indices;
    };

    ListView(HWND hWndList);
    ~ListView();

    void AddColumn(int columnId, const std::wstring& title, int size, ColumnType type);
    HWND GetHandle();
    
    SortOrder GetSortOrder(int columnId);
    void SetSortOrder(int columnId, SortOrder sort);

    std::pair<int, int> GetVisibleIndices();
    void RedrawItems(int first, int last);
    void SetItemCount(int count);

protected:
    virtual std::wstring GetItemText(int columnId, int itemIndex) = 0;

private:
    struct Column;

    static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    CListViewCtrl m_list;
    std::vector<Column> m_cols;

    HWND m_progress;
    HTHEME m_progressTheme;
};
}
