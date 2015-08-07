#include "torrentdetailsframe.h"

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

namespace lt = libtorrent;

TorrentDetailsFrame::TorrentDetailsFrame(wxWindow* parent, wxWindowID id)
    : wxFrame(parent, id, wxT("Details"))
{
}

void TorrentDetailsFrame::SetTorrent(const lt::torrent_status& status)
{
    SetTitle(status.name);
}
