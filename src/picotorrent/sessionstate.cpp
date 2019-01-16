#include "sessionstate.hpp"

#include <libtorrent/torrent_handle.hpp>

namespace lt = libtorrent;
using pt::SessionState;

bool SessionState::isSelected(lt::sha1_hash const& hash)
{
    return selectedTorrents.find(hash) != selectedTorrents.end();
}
