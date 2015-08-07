#ifndef _PT_UI_MAINFRAME_H
#define _PT_UI_MAINFRAME_H

#include <libtorrent/session_handle.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <map>
#include <wx/frame.h>
#include <wx/panel.h>

#include "torrentdetailsframe.h"
#include "torrentlistctrl.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(libtorrent::session_handle& session);

    void AddTorrent(const libtorrent::torrent_status& status);
    void UpdateTorrent(const libtorrent::torrent_status& status);

protected:
    void OnFileAddTorrent(wxCommandEvent& event);
    void OnFileExit(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);

    void OnListItemActivated(wxListEvent& event);
    void OnListItemRightClick(wxListEvent& event);

    void OnTorrentDetailsFrameClose(wxCloseEvent& event, libtorrent::sha1_hash hash);

    enum
    {
        ptID_FILE_ADD_TORRENT,
        ptID_FILE_EXIT = wxID_EXIT
    };

private:
    wxString GetTorrentState(const libtorrent::torrent_status& status);

    libtorrent::session_handle& session_;
    std::map<libtorrent::sha1_hash, long> items_;
    std::map<long, libtorrent::sha1_hash> itemsReverse_;
    std::map<libtorrent::sha1_hash, TorrentDetailsFrame*> details_;

    TorrentListCtrl* torrentList_;

    wxDECLARE_EVENT_TABLE();
};

#endif
