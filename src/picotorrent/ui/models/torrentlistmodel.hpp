#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <map>
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

        void ClearFilter();
        void SetFilter(std::function<bool(BitTorrent::TorrentHandle*)> const& filter);

        int Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column, bool ascending) const wxOVERRIDE;

        unsigned int GetColumnCount() const wxOVERRIDE { return Columns::_Max; }

        wxString GetColumnType(unsigned int column) const wxOVERRIDE;

        unsigned int GetCount() const wxOVERRIDE;

        void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const wxOVERRIDE;

        bool SetValueByRow(const wxVariant& variant, uint32_t row, uint32_t col) wxOVERRIDE { return false; }

    private:
        bool ApplyFilter();

        std::function<bool(BitTorrent::TorrentHandle*)> m_filter;
        std::vector<libtorrent::info_hash_t> m_filtered;
        std::map<libtorrent::info_hash_t, BitTorrent::TorrentHandle*> m_torrents;
    };
}
}
}
