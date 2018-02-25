#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxNotebook;

namespace libtorrent
{
    struct torrent_handle;
}

namespace pt
{
    class FilesPage;
    class OverviewPage;
    class PeersPage;
    struct SessionState;
    class TrackersPage;
    class Translator;

    class TorrentDetailsView : public wxPanel
    {
    public:
        TorrentDetailsView(wxWindow* parent,
            std::shared_ptr<Translator> translator,
            std::shared_ptr<SessionState> state);
        virtual wxSize GetMinSize() const;

        void Clear();
        void Update();

    private:
        wxNotebook* m_notebook;
        OverviewPage* m_overview;
        FilesPage* m_files;
        PeersPage* m_peers;
        TrackersPage* m_trackers;

        std::shared_ptr<SessionState> m_state;
    };
}
