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
    class PeersViewModel;
    class Translator;

    class PeersPage : public wxPanel
    {
    public:
        PeersPage(wxWindow* parent, wxWindowID id, std::shared_ptr<Translator> translator);

        void Clear();
        void Update(libtorrent::torrent_status const& ts);

    private:
        wxDataViewCtrl* m_peersView;
        PeersViewModel* m_viewModel;
    };
}
