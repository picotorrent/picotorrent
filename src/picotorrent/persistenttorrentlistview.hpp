#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/persist/window.h>

#include "torrentlistview.hpp"

namespace pt
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

    PersistentTorrentListView* wxCreatePersistentObject(TorrentListView* pdvc);
}
