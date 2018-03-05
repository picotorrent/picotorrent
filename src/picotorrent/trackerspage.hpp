#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxDataViewCtrl;
class wxDataViewEvent;

namespace libtorrent
{
    struct torrent_status;
}

namespace pt
{
    class TrackersViewModel;
    class Translator;

    class TrackersPage : public wxPanel
    {
    public:
        TrackersPage(wxWindow* parent, wxWindowID id, std::shared_ptr<Translator> translator);

        void Clear();
        void Update(libtorrent::torrent_status const& ts);

    private:
        struct StatusWrap;

        enum
        {
            ptID_TRACKERS_LIST = wxID_HIGHEST + 2000,
            ptID_ADD,
            ptID_FORCE_REANNOUNCE,
            ptID_REMOVE,
            ptID_COPY_URL
        };

        void OnAddTrackers(wxCommandEvent&);
        void OnCopyUrl(wxCommandEvent&);
        void OnForceReannounce(wxCommandEvent&);
        void OnRemoveTrackers(wxCommandEvent&);
        void OnTrackersContextMenu(wxDataViewEvent&);

        wxDECLARE_EVENT_TABLE();

        wxDataViewCtrl* m_trackersView;
        TrackersViewModel* m_viewModel;

        std::unique_ptr<StatusWrap> m_wrap;
        std::shared_ptr<Translator> m_translator;
    };
}
