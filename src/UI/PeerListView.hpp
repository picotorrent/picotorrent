#pragma once

#include "ListView.hpp"

#include <string>
#include <vector>

namespace Models
{
    struct Peer;
}

namespace UI
{
    class PeerListView : public ListView
    {
    public:
        PeerListView(HWND hWndList);
        ~PeerListView();

        void Add(const Models::Peer& model);
        void Remove(const Models::Peer& model);
        void Update(const Models::Peer& model);

    protected:
        float GetItemProgress(int columnId, int itemIndex);
        std::wstring GetItemText(int columnId, int itemIndex);
        void ShowContextMenu(POINT p, const std::vector<int>& selectedIndices);

    private:
        std::vector<Models::Peer> m_models;
    };
}
