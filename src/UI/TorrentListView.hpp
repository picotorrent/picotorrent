#pragma once

#include "ListView.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ViewModels
{
    class TorrentListViewModel;
}

namespace UI
{
    class TorrentListView : public ListView
    {
    public:
        TorrentListView(HWND hWnd, std::unique_ptr<ViewModels::TorrentListViewModel> model);

    protected:
        float GetItemProgress(int columnId, int itemIndex);
        std::wstring GetItemText(int columnId, int itemIndex);
        void ShowContextMenu(POINT p, const std::vector<int>& selectedIndices);
        bool Sort(int columnId, SortOrder order);

    private:
        std::unique_ptr<ViewModels::TorrentListViewModel> m_model;
    };
}
