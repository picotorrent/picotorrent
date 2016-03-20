#include <picotorrent/server/net/messages/torrent_updated_message.hpp>

#include <picotorrent/core/torrent.hpp>

using picotorrent::core::torrent;
using picotorrent::server::net::messages::torrent_updated_message;

torrent_updated_message::torrent_updated_message(const std::shared_ptr<torrent> &torrent)
    : torrent_(torrent)
{
}

torrent_updated_message::~torrent_updated_message()
{
}

void torrent_updated_message::write_to(std::ostream &stream) const
{
    stream << "Hello!";
}
