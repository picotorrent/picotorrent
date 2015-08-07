#include "picotorrent.h"
#include "common.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>

#include "fsutil.h"

namespace fs = boost::filesystem;
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

    LoadState();
    LoadTorrents();

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

void PicoTorrent::LoadState()
{
    // Load state
    fs::path dataPath(FsUtil::GetDataPath());
    fs::path sessionState = dataPath / ".session_state";

    if (fs::exists(sessionState))
    {
        boost::system::error_code ec;
        uintmax_t size = fs::file_size(sessionState, ec);

        if (ec)
        {
            // Log error
            return;
        }

        std::vector<char> buffer;
        FsUtil::ReadFile(sessionState.string(), buffer);

        lt::bdecode_node node;
        lt::error_code decodeError;
        lt::bdecode(&buffer[0], &buffer[0] + buffer.size(), node, decodeError);

        if (decodeError)
        {
            // Log error
            return;
        }

        session_->load_state(node);
    }
}

void PicoTorrent::LoadTorrents()
{
    fs::path torrentsPath(FsUtil::GetDataPath());
    torrentsPath /= "torrents";

    if (!fs::exists(torrentsPath)
        || !fs::is_directory(torrentsPath))
    {
        return;
    }

    for (fs::directory_entry& entry : fs::directory_iterator(torrentsPath))
    {
        if (entry.path().extension() != ".torrent")
        {
            continue;
        }

        lt::add_torrent_params p;
        p.flags |= lt::add_torrent_params::flag_use_resume_save_path;
        p.save_path = "C:/Temp";
        p.ti = boost::make_shared<lt::torrent_info>(entry.path().string());

        fs::path resumeDataPath(entry.path());
        resumeDataPath.replace_extension(".resume");

        if (fs::exists(resumeDataPath))
        {
            FsUtil::ReadFile(resumeDataPath.string(), p.resume_data);
        }

        session_->async_add_torrent(p);
    }
}

void PicoTorrent::SaveState()
{
    fs::path dataPath("C:/ProgramData/PicoTorrent");

    if (!fs::exists(dataPath))
    {
        fs::create_directories(dataPath);
    }

    fs::path sessionState = dataPath / ".session_state";

    lt::entry state;
    session_->save_state(state);

    std::vector<char> buffer;
    lt::bencode(std::back_inserter(buffer), state);

    std::ofstream output(sessionState.string(), std::ios::binary);
    output.write(&buffer[0], buffer.size());
}

void PicoTorrent::SaveTorrents()
{
    fs::path torrentsPath("C:/ProgramData/PicoTorrent/torrents");

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

    fs::path torrentsPath(FsUtil::GetDataPath());
    torrentsPath /= "torrents";

    if (!fs::exists(torrentsPath))
    {
        fs::create_directories(torrentsPath);
    }

    fs::path resumeDataPath = torrentsPath / (hash + ".torrent");
    std::ofstream output(resumeDataPath.string(), std::ios::binary);
    output.write(&buffer[0], buffer.size());
}
