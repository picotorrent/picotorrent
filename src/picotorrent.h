#ifndef _PT_PICOTORRENT_H
#define _PT_PICOTORRENT_H

#include <wx/app.h>
#include <wx/timer.h>

#include <libtorrent/session.hpp>

#include "scripting/pyhost.h"
#include "ui/mainframe.h"

class PicoTorrent : public wxApp
{
public:
    PicoTorrent();
    ~PicoTorrent();

    virtual bool OnInit();

    virtual int OnExit();

    void SetApplicationStatusText(const wxString& text);

protected:
    void OnSessionAlert();

    void OnSessionTimer(wxTimerEvent& event);

    void OnReadAlerts(wxCommandEvent& event);

    enum
    {
        Session_Timer
    };

private:
    void LoadState();
    void LoadTorrents();
    void SaveState();
    void SaveTorrents();

    void SaveTorrentFile(boost::shared_ptr<const libtorrent::torrent_info> file);
    void DeleteTorrentFile(const libtorrent::sha1_hash& hash);
    void DeleteResumeData(const libtorrent::sha1_hash& hash);

    int numOutstandingResumeData = 0;

    std::unique_ptr<PyHost> pyHost_;
    libtorrent::session* session_;
    MainFrame* mainFrame_;
    wxTimer* timer_;

    wxDECLARE_EVENT_TABLE();
};

#endif
