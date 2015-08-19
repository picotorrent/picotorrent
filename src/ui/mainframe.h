#ifndef _PT_UI_MAINFRAME_H
#define _PT_UI_MAINFRAME_H

#include <map>
#include <vector>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/panel.h>

#include "torrentdetailsframe.h"

namespace libtorrent
{
    class sha1_hash;
    struct torrent_status;
}

class MainFrame : public wxFrame
{
public:
    MainFrame();

    void AddTorrent(const libtorrent::torrent_status& status);
    void UpdateTorrents(std::map<libtorrent::sha1_hash, libtorrent::torrent_status> status);
    void RemoveTorrent(const libtorrent::sha1_hash& hash);

protected:
    void OnFileAddTorrent(wxCommandEvent& event);
    void OnFileExit(wxCommandEvent& event);
    void OnTorrentContextMenu(wxCommandEvent& event);

    void OnSize(wxSizeEvent& event);

    void OnListItemActivated(wxListEvent& event);
    void OnListItemRightClick(wxListEvent& event);

    void OnTorrentDetailsFrameClose(wxCloseEvent& event, libtorrent::sha1_hash hash);

    void ShowDetails(const libtorrent::sha1_hash& hash);

    enum
    {
        ptID_MAINFRAME = 5999,

        ptID_TORRENT_RESUME = 6000,
        ptID_TORRENT_PAUSE = 6001,
        ptID_TORRENT_AUTO_MANAGE_TOGGLE = 6002,
        ptID_TORRENT_FORCE_RECHECK = 6003,
        ptID_TORRENT_MOVE = 6004,
        ptID_TORRENT_REMOVE = 6005,
        ptID_TORRENT_REMOVE_DATA = 6006,
        ptID_TORRENT_SHOW_DETAILS = 6007
    };

private:
    wxString GetTorrentState(const libtorrent::torrent_status& status);

    std::map<libtorrent::sha1_hash, TorrentDetailsFrame*> details_;

    wxListCtrl* torrentList_;

    wxDECLARE_EVENT_TABLE();
};

#endif
