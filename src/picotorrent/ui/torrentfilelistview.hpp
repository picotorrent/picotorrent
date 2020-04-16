#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dataview.h>

namespace pt
{
namespace UI
{
    class TorrentFileListView : public wxDataViewCtrl
    {
    public:
        TorrentFileListView(wxWindow* parent, wxWindowID id);
        virtual ~TorrentFileListView();
    };
}
}
