#include "sessionunloader.hpp"

#include "environment.hpp"
#include "sessionstate.hpp"

#include <filesystem>
#include <fstream>
#include <queue>
#include <sstream>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/write_resume_data.hpp>

using pt::SessionUnloader;

void SessionUnloader::Unload(std::shared_ptr<pt::SessionState> state, std::shared_ptr<pt::Environment> env)
{
    fs::path dataDirectory = env->GetApplicationDataPath();
    if (!fs::exists(dataDirectory)) { fs::create_directories(dataDirectory); }
    fs::path stateFile = dataDirectory / "Session.dat";

    // Save session state
    lt::entry entry;
    state->session->save_state(entry);

    std::ofstream stateStream(stateFile, std::ios::binary);
    lt::bencode(std::ostreambuf_iterator<char>(stateStream), entry);

    state->session->pause();

    // Save each torrents resume data
    int numOutstandingResumeData = 0;
    int numPaused = 0;
    int numFailed = 0;

    std::vector<lt::torrent_status> temp;
    state->session->get_torrent_status(&temp, [](const lt::torrent_status &st) { return true; });

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

    fs::path torrentsDirectory = dataDirectory / "Torrents";
    if (!fs::exists(torrentsDirectory)) { fs::create_directories(torrentsDirectory); }

    while (numOutstandingResumeData > 0)
    {
        lt::alert const* a = state->session->wait_for_alert(lt::seconds(10));
        if (a == nullptr) { continue; }

        std::vector<lt::alert*> alerts;
        state->session->pop_alerts(&alerts);

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

            std::stringstream hex;
            hex << rd->handle.info_hash();
            fs::path datFile = torrentsDirectory / (hex.str() + ".dat");

            std::ofstream datStream(datFile, std::ios::binary);
            lt::bencode(std::ostreambuf_iterator<char>(datStream), dat);
        }
    }
}
