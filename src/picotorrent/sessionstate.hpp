#pragma once

#include <map>
#include <memory>
#include <vector>

#include <libtorrent/sha1_hash.hpp>

namespace libtorrent
{
    class session;
    struct torrent_status;
}

namespace pt
{
    struct SessionState
    {
        std::vector<libtorrent::sha1_hash> loaded_torrents;
        std::unique_ptr<libtorrent::session> session;
        std::map<libtorrent::sha1_hash, libtorrent::torrent_status> torrents;
    };
}
