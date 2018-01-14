#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

class wxDataViewCtrl;

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
        wxDataViewCtrl* m_trackersView;
        TrackersViewModel* m_viewModel;
    };
}
