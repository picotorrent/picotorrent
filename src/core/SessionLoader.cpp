#include "SessionLoader.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <queue>

#include "../Configuration.hpp"
#include "../Environment.hpp"
#include "../Log.hpp"
#include "SessionSettings.hpp"

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using Core::SessionLoader;

struct SessionLoadItem
{
    SessionLoadItem(const std::wstring &p)
        : path(p)
    {
    }

    std::wstring path;

    std::string resume_data;
    std::string magnet_save_path;
    std::string magnet_url;
};

SessionLoader::State SessionLoader::Load()
{
    // Paths
    fs::path data_path = Environment::GetDataPath();
    fs::path state_file = data_path / "Session.dat";
    fs::path torrents_dir = data_path / "Torrents";

    lt::settings_pack settings = SessionSettings::Get();

    State state;
    state.session = std::make_shared<lt::session>(settings);

    // Load state

    if (fs::exists(state_file))
    {
        std::ifstream state_input(state_file, std::ios::binary);
        std::stringstream ss;
        ss << state_input.rdbuf();
        std::string buf = ss.str();

        lt::bdecode_node node;
        lt::error_code ltec;
        lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ltec);

        if (ltec)
        {
            LOG(Error) << "Could not bdecode session state file, error: " << ltec.message();
        }
        else
        {
            state.session->load_state(node);
        }
    }

    // Load torrents
    if (fs::exists(torrents_dir))
    {
        typedef std::pair<int64_t, SessionLoadItem> prio_item_t;
        auto comparer = [](const prio_item_t &lhs, const prio_item_t &rhs)
        {
            return lhs.first > rhs.first;
        };

        Configuration& cfg = Configuration::GetInstance();

        std::priority_queue<prio_item_t, std::vector<prio_item_t>, decltype(comparer)> queue(comparer);
        int64_t maxPosition = std::numeric_limits<int64_t>::max();

        for (auto& tmp : fs::directory_iterator(torrents_dir))
        {
            fs::path dat_file = tmp.path();

            if (dat_file.extension() != ".dat")
            {
                continue;
            }

            SessionLoadItem item(dat_file);

            std::ifstream dat_input(dat_file, std::ios::binary);
            std::stringstream ss;
            ss << dat_input.rdbuf();
            item.resume_data = ss.str();

            lt::error_code ltec;
            lt::bdecode_node node;
            lt::bdecode(
                &item.resume_data[0],
                &item.resume_data[0] + item.resume_data.size(),
                node,
                ltec);

            if (ltec)
            {
                LOG(Debug) << "Unable to decode bencoded file '" << dat_file << ", error: " << ltec;
                continue;
            }

            if (node.type() != lt::bdecode_node::type_t::dict_t)
            {
                LOG(Debug) << "File not a bencoded dictionary, '" << dat_file << "'";
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
                LOG(Warning) << "Dat file did not have a corresponding torrent file, deleting.";
                fs::remove(torrent_file);
                continue;
            }

            lt::add_torrent_params params;

            if (!item.resume_data.empty())
            {
                lt::error_code ltec;
                params = lt::read_resume_data(
                    &item.resume_data[0],
                    static_cast<int>(item.resume_data.size()),
                    ltec);

                if (ltec)
                {
                    LOG(Debug) << "Could not decode resume data.";
                }
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
                    LOG(Info) << "Could not decode torrent file '" << torrent_file << "'";
                    continue;
                }

                params.ti = std::make_shared<lt::torrent_info>(node);
                state.muted_hashes.push_back(params.ti->info_hash());
            }

            if (!item.magnet_url.empty())
            {
                lt::error_code ec;
                lt::parse_magnet_uri(item.magnet_url, params, ec);
                params.save_path = item.magnet_save_path;
            }

            state.session->async_add_torrent(params);
            state.loaded_torrents += 1;
        }
    }

    return state;
}
