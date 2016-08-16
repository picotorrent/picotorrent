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

    struct ShowContextMenu
    {
        POINT point;
        std::vector<int> selected_indices;
    };

    ListView(HWND hWndList);
    ~ListView();

    void AddColumn(int columnId, const std::wstring& title, int size, ColumnType type);
    HWND GetHandle();
    std::pair<int, int> GetVisibleIndices();
    void RedrawItems(int first, int last);
    void SetItemCount(int count);

private:
    struct Column;

    static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    CListViewCtrl m_list;
    std::vector<Column> m_cols;

    HWND m_progress;
    HTHEME m_progressTheme;
};
}
