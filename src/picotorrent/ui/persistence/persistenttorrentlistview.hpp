#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/persist/window.h>

#include "../torrentlistview.hpp"

namespace pt
{
namespace UI
{
namespace Persistence
{
    class PersistentTorrentListView : public wxPersistentWindow<TorrentListView>
    {
    public:
        PersistentTorrentListView(TorrentListView* lv);
        virtual wxString GetKind() const wxOVERRIDE;

    protected:
        bool Restore() wxOVERRIDE;
        void Save() const wxOVERRIDE;

    private:
        TorrentListView* m_tlv;
    };
}
}
}

pt::UI::Persistence::PersistentTorrentListView* wxCreatePersistentObject(pt::UI::TorrentListView* pdvc);

