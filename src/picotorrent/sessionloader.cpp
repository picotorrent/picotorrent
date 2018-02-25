#include "sessionloader.hpp"

#include "config.hpp"
#include "environment.hpp"
#include "sessionsettings.hpp"
#include "sessionstate.hpp"

#include <filesystem>
#include <fstream>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <queue>

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using pt::SessionLoader;

struct SessionLoadItem
{
    SessionLoadItem(fs::path const& p)
        : path(p)
    {
    }

    fs::path path;

    std::vector<char> resume_data;
    std::string magnet_save_path;
    std::string magnet_url;
};

std::shared_ptr<pt::SessionState> SessionLoader::Load(std::shared_ptr<pt::Environment> env, std::shared_ptr<pt::Configuration> cfg)
{
    // Paths
    fs::path dataDirectory = env->GetApplicationDataPath();
    fs::path stateFile = dataDirectory / "Session.dat";
    fs::path torrentsDirectory = dataDirectory / "Torrents";

    lt::settings_pack settings = SessionSettings::Get(cfg);

    std::shared_ptr<SessionState> state = std::make_shared<SessionState>();
    state->session = std::make_unique<lt::session>(settings);

    // Load state
    if (fs::exists(stateFile))
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
    }

    // Load torrents
    if (fs::exists(torrentsDirectory))
    {
        typedef std::pair<int64_t, SessionLoadItem> prio_item_t;
        auto comparer = [](const prio_item_t &lhs, const prio_item_t &rhs)
        {
            return lhs.first > rhs.first;
        };

        std::priority_queue<prio_item_t, std::vector<prio_item_t>, decltype(comparer)> queue(comparer);
        int64_t maxPosition = std::numeric_limits<int64_t>::max();

        for (auto& tmp : fs::directory_iterator(torrentsDirectory))
        {
            fs::path datFile = tmp.path();

            if (datFile.extension() != ".dat")
            {
                continue;
            }


            std::ifstream datStream(datFile, std::ios::binary | std::ios::in);
            
            SessionLoadItem item(datFile);
            std::stringstream ss;
            ss << datStream.rdbuf();
            std::string c = ss.str();
            item.resume_data.assign(c.begin(), c.end());

            lt::error_code ltec;
            lt::bdecode_node node = lt::bdecode(item.resume_data, ltec);

            if (ltec || node.type() != lt::bdecode_node::type_t::dict_t)
            {
                continue;
            }

            item.magnet_save_path = node.dict_find_string_value("pT-magnet-savePath").to_string();
            item.magnet_url = node.dict_find_string_value("pT-magnet-url").to_string();

            int64_t queuePosition = node.dict_find_int_value("pT-queuePosition", maxPosition);
            if (queuePosition < 0) { queuePosition = maxPosition; }

            queue.push({ queuePosition, item });
        }

        while (!queue.empty())
        {
            SessionLoadItem item = queue.top().second;
            queue.pop();

            fs::path torrent_file = fs::path(item.path).replace_extension(".torrent");

            if (!fs::exists(torrent_file)
                && item.magnet_url.empty())
            {
                fs::remove(torrent_file);
                continue;
            }

            lt::add_torrent_params params;

            if (!item.resume_data.empty())
            {
                lt::error_code ltec;
                params = lt::read_resume_data(
                    item.resume_data,
                    ltec);
            }

            if (fs::exists(torrent_file))
            {

                std::ifstream torrent_input(torrent_file, std::ios::binary);
                std::stringstream ss;
                ss << torrent_input.rdbuf();
                std::string torrent_buf = ss.str();

                lt::bdecode_node node;
                lt::error_code ltec;
                lt::bdecode(
                    &torrent_buf[0],
                    &torrent_buf[0] + torrent_buf.size(),
                    node,
                    ltec);

                if (ltec)
                {
                    continue;
                }

                params.ti = std::make_shared<lt::torrent_info>(node);
                state->loaded_torrents.push_back(params.ti->info_hash());
            }

            if (!item.magnet_url.empty())
            {
                lt::error_code ec;
                lt::parse_magnet_uri(item.magnet_url, params, ec);
                params.save_path = item.magnet_save_path;
            }

            state->session->async_add_torrent(params);
        }
    }

    return state;
}
