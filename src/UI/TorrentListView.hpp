#pragma once

#include "ListView.hpp"

#include <map>
#include <string>
#include <vector>

namespace Models { struct Torrent; }

namespace UI
{
    class TorrentListView : public ListView
    {
    public:
        TorrentListView(HWND hWndList);
        ~TorrentListView();

        void Add(const Models::Torrent& model);
        std::vector<Models::Torrent> GetSelectedTorrents();
        void Remove(const Models::Torrent& model);
        void Update(const Models::Torrent& model);

    protected:
        int GetItemIconIndex(int itemIndex);
        float GetItemProgress(int columnId, int itemIndex);
        std::wstring GetItemText(int columnId, int itemIndex);
        void ShowContextMenu(POINT p, const std::vector<int>& selectedIndices);
        bool Sort(int columnId, SortOrder order);

    private:
        HIMAGELIST m_icons;
        std::map<int, int> m_icon_map;
        std::vector<Models::Torrent> m_models;
    };
}
