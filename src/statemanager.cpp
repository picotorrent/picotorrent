#include "statemanager.h"

#pragma warning(disable: 4005 4245 4267 4800)
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session_handle.hpp>
#pragma warning(default: 4005 4245 4267 4800)

#include "path.h"
#include "io/file.h"

namespace fs = boost::filesystem;
namespace lt = libtorrent;
using namespace pico;

StateManager::StateManager(libtorrent::session_handle& session)
    : session_(session)
{
    LoadState();
    LoadTorrents();
}

StateManager::~StateManager()
{
    typedef boost::function<void()> notify_func_t;

    try
    {
        BOOST_LOG_TRIVIAL(info) << "Saving state.";

        session_.set_alert_notify(notify_func_t());
        session_.pause();

        SaveTorrents();
        SaveState();

        BOOST_LOG_TRIVIAL(info) << "State saved.";
    }
    catch (const std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error)
            << "Could not save state: "
            << e.what() << ".";
    }
}

void StateManager::LoadState()
{
    fs::path sessionState = Path::GetStatePath();

    if (!fs::exists(sessionState))
    {
        return;
    }

    BOOST_LOG_TRIVIAL(info)
        << "Loading session state from "
        << sessionState << ".";

    std::vector<char> buffer;
    io::File::ReadBuffer(sessionState.string(), buffer);

    lt::bdecode_node node;
    lt::error_code ec;
    lt::bdecode(&buffer[0], &buffer[0] + buffer.size(), node, ec);

    if (ec)
    {
        BOOST_LOG_TRIVIAL(error)
            << "Could not load session state: "
            << ec.message() << ".";
        return;
    }

    session_.load_state(node);
}

void StateManager::LoadTorrents()
{
    fs::path torrents = Path::GetTorrentsPath();

    if (!fs::exists(torrents) || !fs::is_directory(torrents))
    {
        return;
    }

    BOOST_LOG_TRIVIAL(info)
        << "Loading torrents from "
        << torrents << ".";

    for (fs::directory_entry& entry : fs::directory_iterator(torrents))
    {
        fs::path torrentFile = entry.path();

        if (torrentFile.extension() != ".torrent")
        {
            continue;
        }

        lt::add_torrent_params p;
        p.flags |= lt::add_torrent_params::flag_use_resume_save_path;
        p.save_path = Path::GetDefaultDownloadsPath().string();

        std::vector<char> buffer;
        io::File::ReadBuffer(torrentFile.string(), buffer);

        lt::bdecode_node node;
        lt::error_code ec;
        lt::bdecode(&buffer[0], &buffer[0] + buffer.size(), node, ec);

        if (ec)
        {
            BOOST_LOG_TRIVIAL(error)
                << "Could not load torrent "
                << entry << ": "
                << ec;
            continue;
        }

        torrentFile.replace_extension(".dat");
        if (fs::exists(torrentFile))
        {
            io::File::ReadBuffer(torrentFile.string(), p.resume_data);
        }

        p.ti = boost::make_shared<lt::torrent_info>(entry.path().string());
        session_.async_add_torrent(p);
    }
}

void StateManager::SaveState()
{
    lt::entry state;
    session_.save_state(state);

    std::vector<char> buffer;
    lt::bencode(std::back_inserter(buffer), state);

    fs::path sessionState = Path::GetStatePath();
    io::File::WriteBuffer(sessionState.string(), buffer);

    BOOST_LOG_TRIVIAL(info)
        << "Saved session state to "
        << sessionState << ".";
}

void StateManager::SaveTorrents()
{
    fs::path torrentsPath = Path::GetTorrentsPath();

    if (!fs::exists(torrentsPath))
    {
        fs::create_directories(torrentsPath);
    }

    int numOutstandingResumeData = 0;
    int numFailed = 0;
    int numPaused = 0;

    std::vector<lt::torrent_status> temp;
    session_.get_torrent_status(&temp, [](const lt::torrent_status&) { return true; }, 0);

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

    BOOST_LOG_TRIVIAL(info)
        << "Saving resume data for "
        << std::to_string(numOutstandingResumeData) << " torrent(s)"
        << " to " << torrentsPath << ".";

    while (numOutstandingResumeData > 0)
    {
        const lt::alert* a = session_.wait_for_alert(lt::seconds(10));
        if (a == 0) { continue; }

        std::vector<lt::alert*> alerts;
        session_.pop_alerts(&alerts);

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
            std::string hash = lt::to_hex(h.info_hash().to_string());

            std::vector<char> buffer;
            lt::bencode(std::back_inserter(buffer), *rd->resume_data);

            fs::path resumeDataPath = torrentsPath / (hash + ".dat");
            io::File::WriteBuffer(resumeDataPath.string(), buffer);
        }
    }
}
