#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>

namespace pt
{
namespace BitTorrent
{
    class TorrentHandle;
}
namespace UI
{
    class TorrentContextMenu : public wxMenu
    {
    public:
        TorrentContextMenu(wxWindow* parent, std::vector<BitTorrent::TorrentHandle*> const&);

    private:
        enum
        {
            ptID_RESUME = wxID_HIGHEST + 1,
            ptID_RESUME_FORCE,
            ptID_PAUSE,
            ptID_MOVE,
            ptID_REMOVE,
            ptID_REMOVE_FILES,
            ptID_QUEUE_UP,
            ptID_QUEUE_DOWN,
            ptID_QUEUE_TOP,
            ptID_QUEUE_BOTTOM,
            ptID_COPY_INFO_HASH,
            ptID_OPEN_IN_EXPLORER,
            ptID_FORCE_RECHECK,
            ptID_FORCE_REANNOUNCE,
            ptID_SEQUENTIAL_DOWNLOAD
        };

        wxWindow* m_parent;
    };
}
}
