#include "SessionLoader.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>

#include <queue>

#include "../Configuration.hpp"
#include "../Environment.hpp"
#include "../IO/Directory.hpp"
#include "../IO/File.hpp"
#include "../IO/Path.hpp"
#include "../Log.hpp"
#include "SessionSettings.hpp"

namespace lt = libtorrent;
using Core::SessionLoader;

struct SessionLoadItem
{
    SessionLoadItem(const std::wstring &p)
        : path(p)
    {
    }

    std::wstring path;

    std::vector<char> resume_data;
    std::string magnet_save_path;
    std::string magnet_url;
};

SessionLoader::State SessionLoader::Load()
{
    // Paths
    std::wstring data_path = Environment::GetDataPath();
    std::wstring state_file = IO::Path::Combine(data_path, TEXT("Session.dat"));
    std::wstring torrents_dir = IO::Path::Combine(data_path, TEXT("Torrents"));

    lt::settings_pack settings = SessionSettings::Get();

    State state;
    state.session = std::make_shared<lt::session>(settings);

    // Load state

    if (IO::File::Exists(state_file))
    {
        std::error_code ec;
        std::vector<char> buf = IO::File::ReadAllBytes(state_file, ec);

        if (ec)
        {
            LOG(Error) << "Could not read session state file, error: " << ec.message();
        }
        else
        {
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
    }

    // Load torrents
    if (IO::Directory::Exists(torrents_dir))
    {
        std::vector<std::wstring> torrent_dat_files = IO::Directory::GetFiles(torrents_dir, TEXT("*.dat"));
        LOG(Info) << "Loading " << torrent_dat_files.size() << " torrent(s)";

        typedef std::pair<int64_t, SessionLoadItem> prio_item_t;
        auto comparer = [](const prio_item_t &lhs, const prio_item_t &rhs)
        {
            return lhs.first > rhs.first;
        };

        Configuration& cfg = Configuration::GetInstance();

        std::priority_queue<prio_item_t, std::vector<prio_item_t>, decltype(comparer)> queue(comparer);
        int64_t maxPosition = std::numeric_limits<int64_t>::max();

        for (std::wstring& dat_file : torrent_dat_files)
        {
            SessionLoadItem item(dat_file);

            std::error_code ec;
            item.resume_data = IO::File::ReadAllBytes(dat_file, ec);

            if (ec)
            {
                LOG(Debug) << "Could not read file '" << dat_file << "', error: " << ec;
                continue;
            }

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

            std::wstring torrent_file = IO::Path::ReplaceExtension(item.path, TEXT(".torrent"));

            if (!IO::File::Exists(torrent_file)
                && item.magnet_url.empty())
            {
                LOG(Warning) << "Dat file did not have a corresponding torrent file, deleting.";
                IO::File::Delete(torrent_file);
                continue;
            }

            lt::add_torrent_params params;

            if (!item.resume_data.empty())
            {
                lt::error_code ltec;
                params = lt::read_resume_data(
                    &item.resume_data[0],
                    (int)item.resume_data.size(),
                    ltec);

                if (ltec)
                {
                    LOG(Debug) << "Could not decode resume data.";
                }
            }

            if (IO::File::Exists(torrent_file))
            {
                std::error_code ec;
                std::vector<char> torrent_buf = IO::File::ReadAllBytes(torrent_file, ec);

                if (ec)
                {
                    LOG(Info) << "Could not read torrent file '" << torrent_file << "'";
                    continue;
                }

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
