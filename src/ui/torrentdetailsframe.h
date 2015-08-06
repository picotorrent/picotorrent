#ifndef _PT_UI_TORRENTDETAILSFRAME_H
#define _PT_UI_TORRENTDETAILSFRAME_H

#include <libtorrent/torrent_handle.hpp>
#include <wx/frame.h>

class TorrentDetailsFrame : public wxFrame
{
public:
    TorrentDetailsFrame(wxWindow* parent, wxWindowID id);

    void SetTorrent(const libtorrent::torrent_status& status);
};

#endif
