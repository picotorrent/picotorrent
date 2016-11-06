#pragma once

#include "ListView.hpp"

#include <vector>

#include <windows.h>

namespace Models
{
    struct Tracker;
}

namespace UI
{
    class TrackerListView : public ListView
    {
    public:
        TrackerListView(HWND hWndParent);
        ~TrackerListView();

        void Add(const Models::Tracker& tracker);
        void Update(const Models::Tracker& tracker);
        void Remove(const Models::Tracker& tracker);
        void RemoveAll();

    protected:
        std::wstring GetItemText(int columnId, int itemIndex);
        void ShowContextMenu(POINT p, const std::vector<int>& selectedIndices);

    private:
        std::vector<Models::Tracker> m_trackers;
    };
}
