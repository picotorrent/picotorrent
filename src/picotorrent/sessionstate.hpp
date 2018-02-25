#pragma once

#include <map>
#include <memory>
#include <vector>

#include <libtorrent/sha1_hash.hpp>

namespace libtorrent
{
    class session;
    struct torrent_handle;
}

namespace pt
{
    struct SessionState
    {
        bool IsSelected(libtorrent::sha1_hash const& hash);

        std::vector<libtorrent::sha1_hash> loaded_torrents;
        std::vector<libtorrent::sha1_hash> pause_after_checking;
        std::vector<libtorrent::torrent_handle> selected_torrents;
        std::unique_ptr<libtorrent::session> session;
        std::map<libtorrent::sha1_hash, libtorrent::torrent_handle> torrents;
    };
}
