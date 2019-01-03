#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

#include "peerslistview.hpp"

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
        enum {
            ptID_TORRENT_LIST_VIEW = wxID_HIGHEST + 2
        };
        PeersListView* m_peersView;
        PeersViewModel* m_viewModel;
    };
}
