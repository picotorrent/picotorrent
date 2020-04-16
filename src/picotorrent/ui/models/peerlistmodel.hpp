#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <libtorrent/fwd.hpp>
#include <wx/dataview.h>

#include <vector>

namespace pt
{
namespace UI
{
namespace Models
{
    class PeerListModel : public wxDataViewVirtualListModel
    {
    public:
        enum Column
        {
            IP,
            Client,
            Flags,
            DownloadRate,
            UploadRate,
            Progress,
            _Max
        };

        PeerListModel();
        virtual ~PeerListModel();

        void ResetPeers();
        void Update(std::vector<libtorrent::peer_info> const&);

    private:
        unsigned int GetColumnCount() const wxOVERRIDE;
        wxString GetColumnType(unsigned int col) const wxOVERRIDE;
        void GetValueByRow(wxVariant &variant, unsigned row, unsigned col) const wxOVERRIDE;
        bool SetValueByRow(const wxVariant &variant, unsigned row, unsigned col) wxOVERRIDE;

        std::vector<libtorrent::peer_info> m_data;
    };
}
}
}
