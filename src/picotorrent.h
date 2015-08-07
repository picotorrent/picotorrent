#ifndef _PT_PICOTORRENT_H
#define _PT_PICOTORRENT_H

#include <wx/app.h>
#include <wx/timer.h>

#include <libtorrent/session.hpp>

#include "ui/mainframe.h"

class PicoTorrent : public wxApp
{
public:
    PicoTorrent();
    ~PicoTorrent();

    virtual bool OnInit();

    virtual int OnExit();

    void OnReadAlerts(wxCommandEvent& event);

protected:
    void OnSessionAlert();

    void OnSessionTimer(wxTimerEvent& event);

    void SetApplicationStatusText(const wxString& text);

    enum
    {
        Session_Timer
    };

private:
    libtorrent::session* session_;
    MainFrame* mainFrame_;
    wxTimer* timer_;

    wxDECLARE_EVENT_TABLE();
};

#endif
