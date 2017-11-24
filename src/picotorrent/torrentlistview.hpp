#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <wx/dataview.h>

namespace pt
{
    class Translator;

    class TorrentListView : public wxDataViewCtrl
    {
    public:
        TorrentListView(wxWindow* parent, wxWindowID id, std::shared_ptr<Translator> translator);
        virtual wxSize GetMinSize() const;
    };
}
