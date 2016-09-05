#pragma once

#include "../stdafx.h"

#include <string>
#include <vector>

namespace UI
{
class ListView : public CListViewCtrl
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

    ListView(HWND hWndList);
    ~ListView();

    void AddColumn(int columnId, const std::wstring& title, int size, ColumnType type);

    SortOrder GetSortOrder(int columnId);
    void SetSortOrder(int columnId, SortOrder sort);

    std::pair<int, int> GetVisibleIndices();

protected:
    void SendCommand(UINT uMsg, LPARAM lParam);

    virtual float GetItemProgress(int columnId, int itemIndex) { return -1; }
    virtual std::wstring GetItemText(int columnId, int itemIndex) = 0;
    virtual void ShowContextMenu(POINT p, const std::vector<int>& selectedIndices) { };
    virtual bool Sort(int columnId, SortOrder order) { return false; }

private:
    bool IsPointInHeader(POINT p);
    void ShowColumnContextMenu(POINT p);

    struct Column;

    static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    std::vector<Column> m_cols;

    HWND m_progress;
    HTHEME m_progressTheme;
};
}
