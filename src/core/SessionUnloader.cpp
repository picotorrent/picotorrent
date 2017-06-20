#include "SessionUnloader.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/write_resume_data.hpp>

#include <filesystem>
#include <fstream>
#include <queue>
#include <sstream>

#include "../Configuration.hpp"
#include "../Environment.hpp"
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

    fs::path data_path = Environment::GetDataPath();
    fs::path state_file = data_path / "Session.dat";

    if (!fs::exists(data_path))
    {
        fs::create_directories(data_path);
    }

    std::ofstream state_stream(state_file, std::ios::binary);
    std::copy(
        buf.begin(),
        buf.end(),
        std::ostreambuf_iterator<char>(state_stream));

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
    fs::path torrents_dir = data_path / "Torrents";

    if (!fs::exists(torrents_dir))
    {
        fs::create_directories(torrents_dir);
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
            fs::path dat_file = torrents_dir / (hex.str() + ".dat");

            std::ofstream dat_stream(state_file, std::ios::binary);
            std::copy(
                buf.begin(),
                buf.end(),
                std::ostreambuf_iterator<char>(dat_stream));
        }
    }
}
