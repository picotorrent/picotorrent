#include "sessionunloader.hpp"

#include "database.hpp"
#include "sessionstate.hpp"

#include <queue>
#include <sstream>
#include <vector>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/write_resume_data.hpp>

using pt::SessionUnloader;

void SessionUnloader::unload(std::shared_ptr<pt::SessionState> state, std::shared_ptr<pt::Database> db)
{
    // Save session state
    lt::entry entry;
    state->session->save_state(entry);

    std::vector<char> stateBuffer;
    lt::bencode(std::back_inserter(stateBuffer), entry);

    auto stmt = db->statement("INSERT INTO session_state (state_data, timestamp) VALUES (?, strftime('%s'))");
    stmt->bind(1, stateBuffer);
    stmt->execute();

    state->session->pause();

    // Save each torrents resume data
    int numOutstandingResumeData = 0;
    int numPaused = 0;
    int numFailed = 0;

    std::vector<lt::torrent_status> temp = state->session->get_torrent_status([](const lt::torrent_status &st) { return true; });

    for (lt::torrent_status &st : temp)
    {
        if (!st.handle.is_valid()
            || !st.has_metadata
            || !st.need_save_resume)
        {
            continue;
        }

        st.handle.save_resume_data(
            lt::torrent_handle::flush_disk_cache
            | lt::torrent_handle::save_info_dict);

        ++numOutstandingResumeData;
    }

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

            std::vector<char> buffer = lt::write_resume_data_buf(rd->params);

            std::stringstream ss;
            ss << rd->handle.info_hash();
            std::string ih = ss.str();

            // Store state
            stmt = db->statement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
            stmt->bind(1, ih);
            stmt->bind(2, static_cast<int>(rd->handle.status().queue_position));
            stmt->execute();

            // Store the data
            stmt = db->statement("REPLACE INTO torrent_resume_data (info_hash, resume_data) VALUES (?, ?);");
            stmt->bind(1, ih);
            stmt->bind(2, buffer);
            stmt->execute();
        }
    }
}
