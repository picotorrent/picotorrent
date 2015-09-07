#include "picotorrent.h"

#pragma warning(disable: 4005 4245 4267 4800)
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#pragma warning(default: 4005 4245 4267 4800)

#include "path.h"
#include "statemanager.h"
#include "util.h"
#include "io/file.h"
#include "ui/mainframe.h"

namespace fs = boost::filesystem;
namespace lt = libtorrent;
using namespace pico;

CAppModule _Module;

PicoTorrent::PicoTorrent(HINSTANCE hInstance)
    : singleInstance_(NULL)
{
    BOOST_LOG_TRIVIAL(info) << "PicoTorrent starting up.";

    SetProcessDPIAware();

    _Module.Init(NULL, hInstance);
    _Module.AddMessageLoop(&loop_);

    lt::settings_pack settings;;

    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings.set_str(lt::settings_pack::listen_interfaces, "0.0.0.0:6881");

    session_ = std::make_unique<lt::session>(settings);
    metrics_ = lt::session_stats_metrics();

    frame_ = std::make_shared<MainFrame>(*session_);
}

PicoTorrent::~PicoTorrent()
{
    _Module.RemoveMessageLoop();
    _Module.Term();

    if (singleInstance_ != NULL)
    {
        BOOST_LOG_TRIVIAL(debug) << "Closing single instance mutex.";

        ReleaseMutex(singleInstance_);
        CloseHandle(singleInstance_);
        singleInstance_ = NULL;
    }

    BOOST_LOG_TRIVIAL(info) << "PicoTorrent shutting down.";
}

bool PicoTorrent::Init()
{
    BOOST_LOG_TRIVIAL(debug) << "Trying to acquire mutex.";

    singleInstance_ = CreateMutex(NULL, TRUE, L"PicoTorrent/1.0");
    DWORD err = GetLastError();

    if (err == ERROR_ALREADY_EXISTS)
    {
        BOOST_LOG_TRIVIAL(debug) << "Mutex already exists.";

        CloseHandle(singleInstance_);
        singleInstance_ = NULL;

        HWND otherInstance = FindWindow(L"PicoTorrent", NULL);

        if (otherInstance == NULL)
        {
            err = GetLastError();
            BOOST_LOG_TRIVIAL(warning)
                << "Could not find other instance: "
                << std::hex << err;
        }

        LPWSTR cmd = GetCommandLine();

        COPYDATASTRUCT cds;
        cds.cbData = (DWORD)(sizeof(TCHAR) * (_tcslen(cmd) + 1));
        cds.dwData = 1;
        cds.lpData = cmd;

        BOOST_LOG_TRIVIAL(debug) << "Sending command line to other instance.";
        SendMessage(otherInstance, WM_COPYDATA, (WPARAM)otherInstance, (LPARAM)(LPVOID)&cds);

        return false;
    }

    return true;
}

int PicoTorrent::Run(int nCmdShow)
{
    StateManager sm(*session_);

    frame_->CreateEx();
    frame_->ShowWindow(nCmdShow);
    
    // Start reading the session alerts.
    reader_ = std::thread(&PicoTorrent::ReadAlerts, this);

    int ret = loop_.Run();
    shouldRead_ = false;
    reader_.join();

    return ret;
}

void PicoTorrent::ReadAlerts()
{
    shouldRead_ = true;

    while (shouldRead_)
    {
        if (!session_->wait_for_alert(libtorrent::milliseconds(500)))
        {
            continue;
        }

        std::vector<lt::alert*> alerts;
        session_->pop_alerts(&alerts);

        for (lt::alert* a : alerts)
        {
            HandleAlert(a);
        }
    }
}

void PicoTorrent::HandleAlert(lt::alert* alert)
{
    switch (alert->type())
    {
    case lt::add_torrent_alert::alert_type:
    {
        lt::add_torrent_alert* a = lt::alert_cast<lt::add_torrent_alert>(alert);

        if (a->error)
        {
            BOOST_LOG_TRIVIAL(error)
                << "Could not add torrent: "
                << a->error.message();
            break;
        }

        SaveTorrent(a->handle.torrent_file());

        lt::torrent_status st = a->handle.status();
        frame_->AddTorrent(st);
        break;
    }

    case lt::metadata_received_alert::alert_type:
    {
        lt::metadata_received_alert* a = lt::alert_cast<lt::metadata_received_alert>(alert);
        SaveTorrent(a->handle.torrent_file());
        break;
    }

    case lt::session_stats_alert::alert_type:
    {
        break;
    }

    case lt::state_update_alert::alert_type:
    {
        lt::state_update_alert* a = lt::alert_cast<lt::state_update_alert>(alert);

        int64_t dl = 0;
        int64_t ul = 0;

        for (lt::torrent_status& status : a->status)
        {
            dl += status.download_payload_rate;
            ul += status.upload_payload_rate;

            frame_->UpdateTorrent(status);
        }

        wchar_t title[1024];
        _snwprintf(title,
            _ARRAYSIZE(title),
            L"PicoTorrent [DL:%s] [UL:%s]",
            Util::ToSpeed(dl).c_str(),
            Util::ToSpeed(ul).c_str());

        frame_->SetWindowTextW(title);

        break;
    }
    }
}

void PicoTorrent::SaveTorrent(boost::shared_ptr<const lt::torrent_info> info)
{
    if (!info->is_valid())
    {
        BOOST_LOG_TRIVIAL(error) << "Could not save invalid torrent.";
        return;
    }

    fs::path torrents = Path::GetTorrentsPath();

    if (!fs::exists(torrents))
    {
        fs::create_directories(torrents);
    }

    std::string hash = lt::to_hex(info->info_hash().to_string());
    fs::path torrentFile = torrents / (hash + ".torrent");

    if (fs::exists(torrentFile))
    {
        return;
    }

    lt::create_torrent c(*info);
    lt::entry e = c.generate();

    std::vector<char> buffer;
    lt::bencode(std::back_inserter(buffer), e);

    io::File::WriteBuffer(torrentFile.string(), buffer);

    BOOST_LOG_TRIVIAL(info) << "Saved torrent file " << torrentFile;
}
