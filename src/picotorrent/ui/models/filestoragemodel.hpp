#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <map>
#include <memory>

#include <libtorrent/download_priority.hpp>
#include <libtorrent/fwd.hpp>
#include <wx/dataview.h>

namespace pt
{
namespace UI
{
namespace Models
{
    class FileStorageModel : public wxDataViewModel
    {
    public:
        enum Columns
        {
            Name,
            Size,
            Progress,
            Priority,
            _Max
        };

        FileStorageModel(std::function<void(wxDataViewItemArray&, libtorrent::download_priority_t)> const& priorityChanged = nullptr);

        // Things we need to override
        unsigned int GetColumnCount() const wxOVERRIDE;
        wxString GetColumnType(unsigned int col) const wxOVERRIDE;
        void GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const wxOVERRIDE;
        bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col) wxOVERRIDE;
        wxDataViewItem GetParent(const wxDataViewItem &item) const wxOVERRIDE;
        bool IsContainer(const wxDataViewItem &item) const wxOVERRIDE;
        unsigned int GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const wxOVERRIDE;

        void ClearNodes();
        std::vector<libtorrent::file_index_t> GetFileIndices(wxDataViewItemArray&);
        wxDataViewItem GetRootItem();
        void RebuildTree(std::shared_ptr<const libtorrent::torrent_info> ti);
        void UpdatePriorities(const std::vector<libtorrent::download_priority_t>& priorities);
        void UpdateProgress(std::vector<int64_t> const& progress);

    private:
        struct Node
        {
            std::string name;
            int64_t size;
            libtorrent::file_index_t index;
            libtorrent::download_priority_t priority;
            float progress;

            std::shared_ptr<Node> parent;
            std::map<std::string, std::shared_ptr<Node>> children;
        };

        void FillIndices(Node* node, std::vector<libtorrent::file_index_t>& indices);
        wxIcon GetIconForFile(std::string const& fileName) const;

        std::shared_ptr<Node> m_root;
        std::map<libtorrent::file_index_t, std::shared_ptr<Node>> m_map;
        std::map<std::string, wxIcon> m_icons;

        std::function<void(wxDataViewItemArray&, libtorrent::download_priority_t)> m_priorityChangedCallback;
    };
}
}
}
