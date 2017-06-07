#include "Peer.hpp"

#include <libtorrent/peer_info.hpp>
#include <libtorrent/sha1_hash.hpp>

#include <sstream>

#include "../StringUtils.hpp"

namespace lt = libtorrent;
using Models::Peer;

Peer Peer::Map(const std::wstring& endpoint)
{
    return Peer{ endpoint };
}


Peer Peer::Map(const lt::peer_info& peer)
{
    std::stringstream ip;
    ip << peer.ip;

    std::stringstream flags;

    if (peer.flags & lt::peer_info::interesting)
    {
        if (peer.flags & lt::peer_info::remote_choked)
        {
            flags << "d ";
        }
        else
        {
            flags << "D ";
        }
    }

    if (peer.flags & lt::peer_info::remote_interested)
    {
        if (peer.flags & lt::peer_info::choked)
        {
            flags << "u ";
        }
        else
        {
            flags << "U ";
        }
    }

    if (peer.flags & lt::peer_info::optimistic_unchoke)
    {
        flags << "O ";
    }

    if (peer.flags & lt::peer_info::snubbed)
    {
        flags << "S ";
    }

    if (peer.flags & lt::peer_info::local_connection)
    {
        flags << "I ";
    }

    if (!(peer.flags & lt::peer_info::remote_choked) && !(peer.flags & lt::peer_info::interesting))
    {
        flags << "K ";
    }

    if (!(peer.flags & lt::peer_info::choked) && !(peer.flags & lt::peer_info::remote_interested))
    {
        flags << "? ";
    }

    if (peer.source & lt::peer_info::pex)
    {
        flags << "X ";
    }

    if (peer.source & lt::peer_info::dht)
    {
        flags << "H ";
    }

    if (peer.flags & lt::peer_info::rc4_encrypted)
    {
        flags << "E ";
    }

    if (peer.flags & lt::peer_info::plaintext_encrypted)
    {
        flags << "e ";
    }

    if (peer.flags & lt::peer_info::utp_socket)
    {
        flags << "P ";
    }

    if (peer.source & lt::peer_info::lsd)
    {
        flags << "L ";
    }

    return Peer
    {
        TWS(ip.str()),
        TWS(peer.client),
        TWS(flags.str()),
        peer.payload_down_speed,
        peer.payload_up_speed,
        peer.progress
    };
}
