#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

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
    class FileStorageModel;
}

    class TorrentFileListView;

    class TorrentDetailsFilesPanel : public wxPanel
    {
    public:
        TorrentDetailsFilesPanel(wxWindow* parent, wxWindowID id);

        void Refresh(BitTorrent::TorrentHandle* torrent);
        void Reset();

    private:
        void ShowFileContextMenu(wxCommandEvent&);

        Models::FileStorageModel* m_filesModel;
        TorrentFileListView* m_fileList;
        BitTorrent::TorrentHandle* m_torrent;
    };
}
}
