#include "SessionUnloader.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/write_resume_data.hpp>

#include <queue>

#include "../Configuration.hpp"
#include "../Environment.hpp"
#include "../IO/Directory.hpp"
#include "../IO/File.hpp"
#include "../IO/Path.hpp"
#include "../Log.hpp"
#include "../resources.h"
#include "../StringUtils.hpp"

namespace lt = libtorrent;
using Core::SessionUnloader;

void SessionUnloader::Unload(const std::shared_ptr<lt::session>& session)
{
    // Save session state
    lt::entry entry;
    session->save_state(entry);

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), entry);

    std::wstring data_path = Environment::GetDataPath();
    std::wstring state_file = IO::Path::Combine(data_path, TEXT("Session.dat"));

    if (!IO::Directory::Exists(Environment::GetDataPath()))
    {
        IO::Directory::Create(Environment::GetDataPath());
    }

    std::error_code ec;
    IO::File::WriteAllBytes(state_file, buf, ec);

    if (ec)
    {
        LOG(Warning) << "Could not save session state, error: " << ec;
    }

    session->pause();

    // Save each torrents resume data
    int numOutstandingResumeData = 0;
    int numPaused = 0;
    int numFailed = 0;

    std::vector<lt::torrent_status> temp;
    session->get_torrent_status(&temp, [](const lt::torrent_status &st) { return true; }, 0);

    for (lt::torrent_status &st : temp)
    {
        if (!st.handle.is_valid()
            || !st.has_metadata
            || !st.need_save_resume)
        {
            continue;
        }

        st.handle.save_resume_data();
        ++numOutstandingResumeData;
    }

    LOG(Info) << "Saving resume data for " << numOutstandingResumeData << " torrent(s)";
    std::wstring torrents_dir = IO::Path::Combine(Environment::GetDataPath(), TEXT("Torrents"));

    if (!IO::Directory::Exists(torrents_dir))
    {
        IO::Directory::Create(torrents_dir);
    }

    while (numOutstandingResumeData > 0)
    {
        const lt::alert *a = session->wait_for_alert(lt::seconds(10));
        if (a == 0) { continue; }

        std::vector<lt::alert*> alerts;
        session->pop_alerts(&alerts);

        for (lt::alert *a : alerts)
        {
            lt::torrent_paused_alert *tp = lt::alert_cast<lt::torrent_paused_alert>(a);

            if (tp)
            {
                ++numPaused;
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++numFailed;
                --numOutstandingResumeData;
                continue;
            }

            lt::save_resume_data_alert *rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --numOutstandingResumeData;

            // PicoTorrent state
            lt::entry dat = lt::write_resume_data(rd->params);
            dat.dict().insert({ "pT-queuePosition", rd->handle.status().queue_position });

            std::vector<char> buf;
            lt::bencode(std::back_inserter(buf), dat);

            std::stringstream hex;
            hex << rd->handle.info_hash();
            std::string file_name = hex.str() + ".dat";
            std::wstring dat_file = IO::Path::Combine(torrents_dir, TWS(file_name));

            std::error_code ec;
            IO::File::WriteAllBytes(dat_file, buf, ec);
        }
    }
}
