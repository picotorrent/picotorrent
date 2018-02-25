#include "sessionstate.hpp"

#include <libtorrent/torrent_handle.hpp>

namespace lt = libtorrent;
using pt::SessionState;

bool SessionState::IsSelected(libtorrent::sha1_hash const& hash)
{
    auto it = std::find_if(
        selected_torrents.begin(),
        selected_torrents.end(),
        [hash](libtorrent::torrent_handle const& handle)
    {
        return handle.info_hash() == hash;
    });

    return it != selected_torrents.end();
}
