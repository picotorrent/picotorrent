#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <wx/dataview.h>
#include <wx/persist/window.h>

namespace pt
{
    class Translator;

    class TorrentListView : public wxDataViewCtrl
    {
    public:
        TorrentListView(wxWindow* parent, wxWindowID id, std::shared_ptr<Translator> translator);
        virtual wxSize GetMinSize() const wxOVERRIDE;
    };

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
