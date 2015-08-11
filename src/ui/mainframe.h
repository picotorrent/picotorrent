#ifndef _PT_UI_MAINFRAME_H
#define _PT_UI_MAINFRAME_H

#include <libtorrent/session_handle.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <map>
#include <vector>
#include <wx/frame.h>
#include <wx/panel.h>

#include "torrentdetailsframe.h"
#include "torrentlistctrl.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(libtorrent::session_handle& session);

    void AddTorrent(const libtorrent::torrent_status& status);
    void UpdateTorrents(std::vector<libtorrent::torrent_status> status);
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

        ptID_FILE_ADD_TORRENT = 4000,
        ptID_FILE_EXIT = wxID_EXIT,

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

    libtorrent::session_handle& session_;
    std::map<libtorrent::sha1_hash, libtorrent::torrent_status> torrents_;
    std::map<libtorrent::sha1_hash, TorrentDetailsFrame*> details_;

    TorrentListCtrl* torrentList_;

    wxDECLARE_EVENT_TABLE();
};

#endif
