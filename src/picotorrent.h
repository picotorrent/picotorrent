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
    
    bool Prompt(const wxString& message);

    void SetApplicationStatusText(const wxString& text);

private:
    std::unique_ptr<PyHost> pyHost_;
    MainFrame* mainFrame_;

    wxDECLARE_EVENT_TABLE();
};

#endif
