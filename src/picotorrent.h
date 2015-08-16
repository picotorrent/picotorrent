#ifndef _PT_PICOTORRENT_H
#define _PT_PICOTORRENT_H

#include <map>
#include <memory>
#include <wx/app.h>
#include <wx/listctrl.h>
#include <wx/timer.h>

namespace libtorrent
{
    class sha1_hash;
    struct torrent_status;
}

class MainFrame;
class PyHost;

class PicoTorrent : public wxApp
{
public:
    PicoTorrent();
    ~PicoTorrent();

    virtual bool OnInit();

    virtual int OnExit();

    void AddTorrent(const libtorrent::torrent_status& status);

    void UpdateTorrents(std::map<libtorrent::sha1_hash, libtorrent::torrent_status> status);
    
    bool Prompt(const wxString& message);

    void SetApplicationStatusText(const wxString& text);

protected:
    void OnTorrentItemActivated(wxListEvent& event);
    void OnTorrentItemSelected(wxListEvent& event);

private:
    std::unique_ptr<PyHost> pyHost_;
    MainFrame* mainFrame_;

    wxDECLARE_EVENT_TABLE();
};

#endif
