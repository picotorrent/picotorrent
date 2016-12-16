#pragma once

#include "ListView.hpp"

#include <vector>

namespace Models
{
    struct TorrentFile;
}

namespace UI
{
    class TorrentFileListView : public ListView
    {
    public:
        TorrentFileListView(HWND hWnd, bool showProgress = false);
        ~TorrentFileListView();

        void Add(const Models::TorrentFile& model);
        void RemoveAll();
        void Update(const Models::TorrentFile& model);

    protected:
        bool GetItemIsChecked(int itemIndex);
        float GetItemProgress(int columnId, int itemIndex);
        std::wstring GetItemText(int columnId, int itemIndex);
        void ShowContextMenu(POINT p, const std::vector<int>& selectedIndices);
        bool Sort(int columnId, ListView::SortOrder order);
        void ToggleItemState(const std::vector<int>& selectedIndices);

    private:
        bool m_showProgress;

        std::wstring GetPriorityString(int priority);
        std::vector<Models::TorrentFile> m_models;
    };
}
