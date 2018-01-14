#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <libtorrent/sha1_hash.hpp>
#include <wx/dataview.h>

#include <map>
#include <memory>
#include <vector>

namespace libtorrent
{
    struct torrent_status;
}

namespace pt
{
    class Translator;

    class TorrentListViewModel : public wxDataViewVirtualListModel
    {
    public:
        TorrentListViewModel(std::shared_ptr<Translator> translator);

        void Add(libtorrent::torrent_status const& ts);
        libtorrent::sha1_hash FindHashByRow(int row);
        void Remove(libtorrent::sha1_hash const& hash);
        void Update(libtorrent::torrent_status const& ts);

    private:
        unsigned int GetColumnCount() const wxOVERRIDE;
        wxString GetColumnType(unsigned int col) const wxOVERRIDE;
        void GetValueByRow(wxVariant &variant, unsigned row, unsigned col) const wxOVERRIDE;
        bool SetValueByRow(const wxVariant &variant, unsigned row, unsigned col) wxOVERRIDE;

        std::vector<libtorrent::sha1_hash> m_order;
        std::map<libtorrent::sha1_hash, libtorrent::torrent_status> m_status;
        std::shared_ptr<Translator> m_translator;
    };
}
