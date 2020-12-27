#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <libtorrent/info_hash.hpp>
#include <wx/dataview.h>

namespace pt::BitTorrent
{
    class TorrentHandle;
    struct TorrentStatus;
}

namespace pt::UI::Filters
{
    class TorrentFilter;
}

namespace pt::UI::Models
{
    class TorrentListModel : public wxDataViewIndexListModel
    {
    public:
        enum Columns
        {
            Name,
            QueuePosition,
            Size,
            SizeRemaining,
            Status,
            Progress,
            ETA,
            DownloadSpeed,
            UploadSpeed,
            Availability,
            Ratio,
            Seeds,
            Peers,
            AddedOn,
            CompletedOn,
            Label,
            _Max
        };

        TorrentListModel();
        virtual ~TorrentListModel();

        void AddTorrent(BitTorrent::TorrentHandle* torrent);
        int GetRowIndex(BitTorrent::TorrentHandle* torrent);
        BitTorrent::TorrentHandle* GetTorrentFromItem(wxDataViewItem const& item);
        void RemoveTorrent(libtorrent::info_hash_t const& hash);
        void UpdateTorrents(std::vector<BitTorrent::TorrentHandle*> torrents);
        void SetBackgroundColorEnabled(bool enabled);

        void ClearFilter();
        void ClearLabelFilter();
        void SetFilter(std::unique_ptr<Filters::TorrentFilter> filter);
        void SetLabelFilter(int labelId);

        int Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column, bool ascending) const wxOVERRIDE;

        bool GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr& attr) const wxOVERRIDE;

        unsigned int GetColumnCount() const wxOVERRIDE { return Columns::_Max; }

        wxString GetColumnType(unsigned int column) const wxOVERRIDE;

        unsigned int GetCount() const wxOVERRIDE;

        void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const wxOVERRIDE;

        bool SetValueByRow(const wxVariant&, uint32_t, uint32_t) wxOVERRIDE { return false; }

        void UpdateLabels(std::map<int, std::tuple<std::string, std::string>> const& labels);

    private:
        void ApplyFilter();
        void ApplyFilter(std::vector<BitTorrent::TorrentHandle*> torrents);

        bool m_backgroundColorEnabled;
        int m_filterLabelId;
        std::unique_ptr<Filters::TorrentFilter> m_filter;
        std::vector<libtorrent::info_hash_t> m_filtered;
        std::map<int, std::tuple<std::string, std::string>> m_labels;
        std::map<int, wxColor> m_labelsColors;
        std::map<int, wxIcon> m_labelsIcons;
        std::map<libtorrent::info_hash_t, BitTorrent::TorrentHandle*> m_torrents;
    };
}
