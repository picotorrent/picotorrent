#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dataview.h>

#include <memory>
#include <vector>

namespace libtorrent
{
    struct announce_entry;
    struct torrent_status;
}

namespace pt
{
    class Translator;

    class TrackersViewModel : public wxDataViewVirtualListModel
    {
    public:
        TrackersViewModel(std::shared_ptr<Translator> translator);

        enum Columns
        {
            Url,
            Status,
            Fails,
            NextAnnounce,
            _Max
        };

        void Clear();
        void Update(libtorrent::torrent_status const& ts);

    private:
        unsigned int GetColumnCount() const wxOVERRIDE;
        wxString GetColumnType(unsigned int col) const wxOVERRIDE;
        void GetValueByRow(wxVariant &variant, unsigned row, unsigned col) const wxOVERRIDE;
        bool SetValueByRow(const wxVariant &variant, unsigned row, unsigned col) wxOVERRIDE;

        std::shared_ptr<Translator> m_translator;
        std::vector<libtorrent::announce_entry> m_data;
    };
}
