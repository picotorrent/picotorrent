#pragma once

#include "ListView.hpp"

#include <map>
#include <string>
#include <vector>

struct Torrent;

namespace UI
{
    class TorrentListView : public ListView
    {
    public:
        TorrentListView(HWND hWndList);

        void Add(const Torrent& model);
        std::vector<Torrent> GetSelectedTorrents();
        void Remove(const std::string& infoHash);
        void Update(const Torrent& model);

    protected:
        int GetItemIconIndex(int itemIndex);
        float GetItemProgress(int columnId, int itemIndex);
        std::wstring GetItemText(int columnId, int itemIndex);
        void OnItemActivated(std::vector<int> const& indices);
        void OnDestroy();
        void ShowContextMenu(POINT p, const std::vector<int>& selectedIndices);
        bool Sort(int columnId, SortOrder order);

    private:
        HIMAGELIST m_icons;
        std::map<int, int> m_icon_map;
        std::vector<Torrent> m_models;
    };
}
