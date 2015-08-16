#ifndef _PT_PICOTORRENT_H
#define _PT_PICOTORRENT_H

#include <memory>
#include <wx/app.h>
#include <wx/timer.h>

namespace libtorrent
{
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
    
    bool Prompt(const wxString& message);

    void SetApplicationStatusText(const wxString& text);

private:
    std::unique_ptr<PyHost> pyHost_;
    MainFrame* mainFrame_;

    wxDECLARE_EVENT_TABLE();
};

#endif
