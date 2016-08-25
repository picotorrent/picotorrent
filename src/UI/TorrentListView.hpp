#pragma once

#include "ListView.hpp"

#include <string>
#include <vector>

namespace Models
{
    class Torrent;
}

namespace UI
{
    class TorrentListView : public ListView
    {
    public:
        TorrentListView(HWND hWndList);

        void Add(const Models::Torrent& model);
        void Remove(const Models::Torrent& model);
        void Update(const Models::Torrent& model);

    protected:
        float GetItemProgress(int columnId, int itemIndex);
        std::wstring GetItemText(int columnId, int itemIndex);
        void ShowContextMenu(POINT p, const std::vector<int>& selectedIndices);
        bool Sort(int columnId, SortOrder order);

    private:
        std::vector<Models::Torrent> m_models;
    };
}
