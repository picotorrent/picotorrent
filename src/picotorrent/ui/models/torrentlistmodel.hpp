#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <map>
#include <string>
#include <vector>

#include <libtorrent/info_hash.hpp>
#include <wx/dataview.h>

namespace pt
{
namespace BitTorrent
{
    class TorrentHandle;
    struct TorrentStatus;
}
namespace UI
{
namespace Models
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
        void SetFilter(std::function<bool(BitTorrent::TorrentHandle*)> const& filter);
        void SetLabelFilter(int labelId);

        int Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column, bool ascending) const wxOVERRIDE;

        bool GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr& attr) const wxOVERRIDE;

        unsigned int GetColumnCount() const wxOVERRIDE { return Columns::_Max; }

        wxString GetColumnType(unsigned int column) const wxOVERRIDE;

        unsigned int GetCount() const wxOVERRIDE;

        void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const wxOVERRIDE;

        bool SetValueByRow(const wxVariant& variant, uint32_t row, uint32_t col) wxOVERRIDE { return false; }

        void UpdateLabelColors(std::map<int, std::string> const& colors);

    private:
        void ApplyFilter();
        void ApplyFilter(std::vector<BitTorrent::TorrentHandle*> torrents);

        bool m_backgroundColorEnabled;
        int m_filterLabelId;
        std::function<bool(BitTorrent::TorrentHandle*)> m_filter;
        std::vector<libtorrent::info_hash_t> m_filtered;
        std::map<int, std::string> m_labelColors;
        std::map<libtorrent::info_hash_t, BitTorrent::TorrentHandle*> m_torrents;
    };
}
}
}
