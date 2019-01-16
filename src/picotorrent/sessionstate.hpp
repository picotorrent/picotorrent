#pragma once

#include <map>
#include <memory>
#include <unordered_set>

#include <libtorrent/fwd.hpp>
#include <libtorrent/sha1_hash.hpp>

namespace pt
{
    struct SessionState
    {
        bool isSelected(libtorrent::sha1_hash const& hash);

        std::unordered_set<libtorrent::sha1_hash> pauseAfterChecking;
        std::unordered_set<libtorrent::sha1_hash> selectedTorrents;
        std::unique_ptr<libtorrent::session> session;
        std::map<libtorrent::sha1_hash, libtorrent::torrent_handle> torrents;
    };
}
