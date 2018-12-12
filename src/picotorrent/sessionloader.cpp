#include "sessionloader.hpp"

#include <queue>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/extensions/smart_ban.hpp>
#include <libtorrent/extensions/ut_metadata.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

#include "configuration.hpp"
#include "database.hpp"
#include "sessionsettings.hpp"
#include "sessionstate.hpp"

namespace lt = libtorrent;
using pt::SessionLoader;

std::shared_ptr<pt::SessionState> SessionLoader::load(std::shared_ptr<pt::Database> db, std::shared_ptr<pt::Configuration> cfg)
{
    lt::settings_pack settings;// = SessionSettings::Get(cfg);
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);

    std::shared_ptr<SessionState> state = std::make_shared<SessionState>();
    state->session = std::make_unique<lt::session>(
        settings,
        lt::session_flags_t{ lt::session_handle::add_default_plugins });

    if (cfg->getBool("enable_pex"))
    {
        state->session->add_extension(lt::create_ut_pex_plugin);
    }

    // Load state
    /*if (fs::exists(stateFile))
    {
        std::ifstream state_input(stateFile, std::ios::binary);
        std::stringstream ss;
        ss << state_input.rdbuf();
        std::string buf = ss.str();

        lt::bdecode_node node;
        lt::error_code ltec;
        lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ltec);

        if (!ltec)
        {
            state->session->load_state(node);
        }
    }*/

    const char* sql = "SELECT trd.resume_data AS resume_data FROM torrent t\n"
        "LEFT JOIN torrent_resume_data trd ON t.info_hash = trd.info_hash\n"
        "ORDER BY t.queue_position ASC";

    auto stmt = db->statement(sql);

    while (stmt->read())
    {
        std::vector<char> resume_data;
        stmt->getBlob(0, resume_data);

        lt::add_torrent_params params;

        if (resume_data.size() > 0)
        {
            lt::error_code ec;
            lt::bdecode_node node = lt::bdecode(resume_data, ec);

            if (ec)
            {
                // TODO(log)
            }

            params = lt::read_resume_data(node, ec);

            if (ec)
            {
                // TODO(log)
            }
        }

        state->session->async_add_torrent(params);
    }

    return state;
}
