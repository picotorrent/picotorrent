#pragma once

#include <memory>
#include <vector>

#include <libtorrent/sha1_hash.hpp>

namespace libtorrent
{
    class session;
}

namespace Core
{
    struct SessionLoader
    {
        struct State
        {
            int64_t loaded_torrents;
            std::vector<libtorrent::sha1_hash> muted_hashes;
            std::shared_ptr<libtorrent::session> session;
        };

        static State Load();
    };
}
