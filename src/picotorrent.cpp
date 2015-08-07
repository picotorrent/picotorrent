#include "picotorrent.h"
#include "common.h"

#include <libtorrent/alert_types.hpp>

namespace lt = libtorrent;
typedef boost::function<void()> notify_func_t;

wxBEGIN_EVENT_TABLE(PicoTorrent, wxApp)
    EVT_COMMAND(wxID_ANY, SESSION_ALERT, PicoTorrent::OnReadAlerts)
    EVT_TIMER(PicoTorrent::Session_Timer, PicoTorrent::OnSessionTimer)
wxEND_EVENT_TABLE()

PicoTorrent::PicoTorrent()
{
    session_ = new lt::session(lt::settings_pack(), 0);
    mainFrame_ = new MainFrame(*session_);
    timer_ = new wxTimer(this, Session_Timer);
}

PicoTorrent::~PicoTorrent()
{
    delete session_;
}

bool PicoTorrent::OnInit()
{
    if(!wxApp::OnInit())
    {
        return false;
    }

    // Set up session
    lt::settings_pack settings = session_->get_settings();
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings.set_str(lt::settings_pack::listen_interfaces, "0.0.0.0:6881");
    session_->apply_settings(settings);

    // Connect our notify function
    session_->set_alert_notify(std::bind(&PicoTorrent::OnSessionAlert, this));

    // Show our main frame.
    mainFrame_->Show(true);

    // Start the timer that is responsible for posting
    // session, torrent and dht stats/metrics.
    timer_->Start(1000);

    SetApplicationStatusText("PicoTorrent loaded.");

    return true;
}

int PicoTorrent::OnExit()
{
    session_->set_alert_notify(notify_func_t());

    return wxApp::OnExit();
}

void PicoTorrent::OnReadAlerts(wxCommandEvent& WXUNUSED(event))
{
    std::vector<lt::alert*> alerts;
    session_->pop_alerts(&alerts);

    for (lt::alert* alert : alerts)
    {
        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            lt::add_torrent_alert* a = lt::alert_cast<lt::add_torrent_alert>(alert);

            if (a->error)
            {
                // Log error and continue
                continue;
            }

            lt::torrent_status status = a->handle.status();
            mainFrame_->AddTorrent(status);
            SetApplicationStatusText(wxString::Format("%s added.", status.name));
        }
        break;

        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* a = lt::alert_cast<lt::state_update_alert>(alert);

            for (lt::torrent_status& status : a->status)
            {
                mainFrame_->UpdateTorrent(status);
            }
        }
        break;
        }
    }
}

void PicoTorrent::OnSessionAlert()
{
    wxCommandEvent* ev = new wxCommandEvent(SESSION_ALERT);
    wxQueueEvent(this, ev);
}

void PicoTorrent::OnSessionTimer(wxTimerEvent& WXUNUSED(event))
{
    session_->post_dht_stats();
    session_->post_session_stats();
    session_->post_torrent_updates();
}

void PicoTorrent::SetApplicationStatusText(const wxString& text)
{
    mainFrame_->SetStatusText(text, 0);
}
