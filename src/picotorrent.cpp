#include "picotorrent.h"
#include "common.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <wx/msgdlg.h>

#include "config.h"
#include "fsutil.h"
#include "platform.h"
#include "scripting/pyhost.h"
#include "ui/mainframe.h"

namespace fs = boost::filesystem;
namespace lt = libtorrent;

typedef boost::function<void()> notify_func_t;

wxBEGIN_EVENT_TABLE(PicoTorrent, wxApp)
    EVT_COMMAND(wxID_ANY, SESSION_ALERT, PicoTorrent::OnReadAlerts)
    EVT_TIMER(PicoTorrent::Session_Timer, PicoTorrent::OnSessionTimer)
wxEND_EVENT_TABLE()

PicoTorrent::PicoTorrent()
{
    session_ = boost::make_shared<lt::session>(lt::settings_pack(), 0);
    mainFrame_ = new MainFrame(*session_);
    timer_ = new wxTimer(this, Session_Timer);
    pyHost_ = std::unique_ptr<PyHost>(new PyHost(this));
}

PicoTorrent::~PicoTorrent()
{
}

bool PicoTorrent::OnInit()
{
    if(!wxApp::OnInit())
    {
        return false;
    }

    // Set up session
    Config& cfg = Config::GetInstance();
    std::pair<std::string, int> iface = cfg.GetListenInterface();

    lt::settings_pack settings = session_->get_settings();
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings.set_str(lt::settings_pack::listen_interfaces, iface.first + ":" + std::to_string(iface.second));
    session_->apply_settings(settings);

    // Connect our notify function
    session_->set_alert_notify(std::bind(&PicoTorrent::OnSessionAlert, this));

    // Show our main frame.
    mainFrame_->Show(true);

    // Start the timer that is responsible for posting
    // session, torrent and dht stats/metrics.
    timer_->Start(1000);

    SetApplicationStatusText("PicoTorrent loaded.");

    // Load the python scripting host as the last thing
    pyHost_->Load();

    return true;
}

int PicoTorrent::OnExit()
{
    // Unload all python things
    pyHost_->Unload();

    session_->set_alert_notify(notify_func_t());
    session_->pause();

    SaveTorrents();
    SaveState();

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

            // If the torrent has metadata, save that metadata to the torrents path.
            if (status.has_metadata)
            {
                SaveTorrentFile(status.handle.torrent_file());
            }

            mainFrame_->AddTorrent(status);
            SetApplicationStatusText(wxString::Format("%s added.", status.name));
        }
        break;

        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* a = lt::alert_cast<lt::state_update_alert>(alert);
            mainFrame_->UpdateTorrents(a->status);
        }
        break;

        case lt::torrent_removed_alert::alert_type:
        {
            lt::torrent_removed_alert* a = lt::alert_cast<lt::torrent_removed_alert>(alert);
            mainFrame_->RemoveTorrent(a->info_hash);

            // Remove torrent file and resume data
            DeleteTorrentFile(a->info_hash);
            DeleteResumeData(a->info_hash);
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

boost::shared_ptr<libtorrent::session> PicoTorrent::GetSession()
{
    return session_;
}

bool PicoTorrent::Prompt(const wxString& text)
{
    return wxMessageBox(text, "PicoTorrent", wxOK | wxCANCEL, mainFrame_) == wxOK;
}

void PicoTorrent::SetApplicationStatusText(const wxString& text)
{
    mainFrame_->SetStatusText(text, 0);
}

void PicoTorrent::SaveState()
{
    lt::entry state;
    session_->save_state(state);

    std::vector<char> buffer;
    lt::bencode(std::back_inserter(buffer), state);

    std::ofstream output(".session_state", std::ios::binary);
    output.write(&buffer[0], buffer.size());
}

void PicoTorrent::SaveTorrents()
{
    fs::path torrentsPath("torrents");

    if (!fs::exists(torrentsPath))
    {
        fs::create_directories(torrentsPath);
    }

    int numFailed = 0;
    int numPaused = 0;

    session_->pause();

    std::vector<lt::torrent_status> temp;
    session_->get_torrent_status(&temp, [](const lt::torrent_status& status) { return true; }, 0);

    for (lt::torrent_status& status : temp)
    {
        if (!status.handle.is_valid()
            || !status.has_metadata
            || !status.need_save_resume)
        {
            // Log (skip file which can't save resume)
            continue;
        }

        status.handle.save_resume_data();
        numOutstandingResumeData++;
    }

    while (numOutstandingResumeData > 0)
    {
        const lt::alert* a = session_->wait_for_alert(lt::seconds(10));
        if (a == 0) { continue; }

        std::vector<lt::alert*> alerts;
        session_->pop_alerts(&alerts);

        for (lt::alert* alert : alerts)
        {
            lt::torrent_paused_alert* tp = lt::alert_cast<lt::torrent_paused_alert>(alert);

            if (tp)
            {
                ++numPaused;
                // Log something
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(alert))
            {
                ++numFailed;
                --numOutstandingResumeData;
                continue;
            }

            lt::save_resume_data_alert* rd = lt::alert_cast<lt::save_resume_data_alert>(alert);
            if (!rd) { continue; }
            --numOutstandingResumeData;
            if (!rd->resume_data) { continue; }

            lt::torrent_handle h = rd->handle;
            lt::torrent_status st = h.status();
            std::string hash = lt::to_hex(st.info_hash.to_string());

            std::vector<char> buffer;
            lt::bencode(std::back_inserter(buffer), *rd->resume_data);

            fs::path resumeDataPath = torrentsPath / (hash + ".resume");
            std::ofstream output(resumeDataPath.string(), std::ios::binary);
            output.write(&buffer[0], buffer.size());
        }
    }
}

void PicoTorrent::SaveTorrentFile(boost::shared_ptr<const lt::torrent_info> file)
{
    if (!file)
    {
        // LOg error
        return;
    }

    lt::create_torrent creator(*file);
    lt::entry encoded = creator.generate();

    std::string hash = lt::to_hex(file->info_hash().to_string());

    std::vector<char> buffer;
    lt::bencode(std::back_inserter(buffer), encoded);

    fs::path torrentsPath("torrents");

    if (!fs::exists(torrentsPath))
    {
        fs::create_directories(torrentsPath);
    }

    fs::path resumeDataPath = torrentsPath / (hash + ".torrent");
    std::ofstream output(resumeDataPath.string(), std::ios::binary);
    output.write(&buffer[0], buffer.size());
}

void PicoTorrent::DeleteTorrentFile(const lt::sha1_hash& hash)
{
    fs::path torrentsPath("torrents");

    std::string encodedHash = lt::to_hex(hash.to_string());
    fs::path torrentFilePath = torrentsPath / (encodedHash + ".torrent");

    if (fs::exists(torrentFilePath))
    {
        fs::remove(torrentFilePath);
    }
}

void PicoTorrent::DeleteResumeData(const lt::sha1_hash& hash)
{
    fs::path torrentsPath("torrents");

    std::string encodedHash = lt::to_hex(hash.to_string());
    fs::path torrentFilePath = torrentsPath / (encodedHash + ".resume");

    if (fs::exists(torrentFilePath))
    {
        fs::remove(torrentFilePath);
    }
}
