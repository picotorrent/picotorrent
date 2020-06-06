#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <libtorrent/fwd.hpp>
#include <wx/dataview.h>

#include <chrono>
#include <memory>
#include <vector>

namespace pt
{
namespace BitTorrent
{
    class TorrentHandle;
}
namespace UI
{
namespace Models
{
    class TrackerListModel : public wxDataViewModel
    {
    public:
        enum Column
        {
            Url,
            Status,
            NumDownloaded,
            NumLeeches,
            NumSeeds,
            Fails,
            NextAnnounce,
            _Max
        };

        enum class ListItemStatus
        {
            unknown,
            error,
            working,
            updating,
        };

        struct ListItem
        {
            std::string key;
            int tier;
            ListItemStatus status;
            int numDownloaded;
            int numLeeches;
            int numSeeds;
            std::chrono::seconds nextAnnounce;
            std::wstring errorMessage;
            int fails;
            int failLimit;

            std::shared_ptr<ListItem> parent;
            std::vector<std::shared_ptr<ListItem>> children;
        };

        TrackerListModel();
        virtual ~TrackerListModel();

        // Things we need to override
        unsigned int GetColumnCount() const wxOVERRIDE;
        wxString GetColumnType(unsigned int col) const wxOVERRIDE;
        void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const wxOVERRIDE;
        bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col) wxOVERRIDE;
        wxDataViewItem GetParent(const wxDataViewItem& item) const wxOVERRIDE;
        bool IsContainer(const wxDataViewItem& item) const wxOVERRIDE;
        unsigned int GetChildren(const wxDataViewItem& item, wxDataViewItemArray& array) const wxOVERRIDE;

        wxDataViewItemArray GetTierNodes();
        void ResetTrackers();
        void Update(BitTorrent::TorrentHandle* torrent);

    private:
        std::shared_ptr<ListItem> m_dht;
        std::shared_ptr<ListItem> m_lsd;
        std::shared_ptr<ListItem> m_pex;

        std::vector<std::shared_ptr<ListItem>> m_items;
    };
}
}
}
