#ifndef _PT_PICOTORRENT_H
#define _PT_PICOTORRENT_H

#include <wx/app.h>
#include <wx/timer.h>

#include <boost/shared_ptr.hpp>
#include <libtorrent/session.hpp>

class MainFrame;
class PyHost;

class PicoTorrent : public wxApp
{
public:
    PicoTorrent();
    ~PicoTorrent();

    virtual bool OnInit();

    virtual int OnExit();

    boost::shared_ptr<libtorrent::session> GetSession();

    bool Prompt(const wxString& message);

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
    void SaveState();
    void SaveTorrents();

    void SaveTorrentFile(boost::shared_ptr<const libtorrent::torrent_info> file);
    void DeleteTorrentFile(const libtorrent::sha1_hash& hash);
    void DeleteResumeData(const libtorrent::sha1_hash& hash);

    int numOutstandingResumeData = 0;

    std::unique_ptr<PyHost> pyHost_;
    boost::shared_ptr<libtorrent::session> session_;
    MainFrame* mainFrame_;
    wxTimer* timer_;

    wxDECLARE_EVENT_TABLE();
};

#endif
