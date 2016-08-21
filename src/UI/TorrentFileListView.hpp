#pragma once

#include "ListView.hpp"

#include <memory>

namespace UI
{
    class TorrentFileListView : public ListView
    {
    public:
        struct ViewModel
        {
            virtual std::wstring GetFileName(int index) = 0;
            virtual int64_t GetFileSize(int index) = 0;
            virtual uint8_t GetFilePriority(int index) = 0;
            virtual void SetFilePriority(int index, uint8_t prio) = 0;
        };

        TorrentFileListView(HWND hWnd);
        void UpdateModel(std::unique_ptr<ViewModel> vm);

    protected:
        std::wstring GetItemText(int columnId, int itemIndex);
        void ShowContextMenu(POINT p, const std::vector<int>& selectedIndices);

    private:
        std::wstring GetPriorityString(int priority);
        std::unique_ptr<ViewModel> m_model;
    };
}
