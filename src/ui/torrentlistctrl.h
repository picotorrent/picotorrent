#ifndef _PT_UI_TORRENTLISTCTRL
#define _PT_UI_TORRENTLISTCTRL

#include <wx/listctrl.h>

class TorrentListCtrl : public wxListCtrl
{
public:
    TorrentListCtrl(wxWindow *parent,
           const wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style)
    : wxListCtrl(parent, id, pos, size, style)
    {
    }
};

#endif
