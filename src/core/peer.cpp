#include <picotorrent/core/peer.hpp>

#include <libtorrent/peer_info.hpp>

namespace lt = libtorrent;
using picotorrent::core::peer;

peer::peer(const lt::peer_info &pi)
    : pi_(std::make_shared<lt::peer_info>(pi))
{
}

peer::peer(const peer &that)
    : pi_(that.pi_)
{
}

peer::~peer()
{
}

std::string peer::client() const
{
    return pi_->client;
}

int peer::download_rate() const
{
    return pi_->payload_down_speed;
}

std::string peer::id() const
{
    return ip() + ":" + std::to_string(port());
}

std::string peer::ip() const
{
    return pi_->ip.address().to_string();
}

int peer::port() const
{
    return pi_->ip.port();
}

int peer::upload_rate() const
{
    return pi_->payload_up_speed;
}

std::string peer::flags_str() const
{
    std::string flags = "";

    if (pi_->flags & lt::peer_info::interesting)
    {
        if (pi_->flags & lt::peer_info::remote_choked)
        {
            flags += "d ";
        }
        else
        {
            flags += "D ";
        }
    }

    if (pi_->flags & lt::peer_info::remote_interested)
    {
        if (pi_->flags & lt::peer_info::choked)
        {
            flags += "u ";
        }
        else
        {
            flags += "U ";
        }
    }

    if (pi_->flags & lt::peer_info::optimistic_unchoke)
    {
        flags += "O ";
    }

    if (pi_->flags & lt::peer_info::snubbed)
    {
        flags += "S ";
    }

    if (pi_->flags & lt::peer_info::local_connection)
    {
        flags += "I ";
    }

    if (!(pi_->flags & lt::peer_info::remote_choked) && !(pi_->flags & lt::peer_info::interesting))
    {
        flags += "K ";
    }

    if (!(pi_->flags & lt::peer_info::choked) && !(pi_->flags & lt::peer_info::remote_interested))
    {
        flags += "? ";
    }

    if (pi_->source & lt::peer_info::pex)
    {
        flags += "X ";
    }

    if (pi_->source & lt::peer_info::dht)
    {
        flags += "H ";
    }

    if (pi_->flags & lt::peer_info::rc4_encrypted)
    {
        flags += "E ";
    }

    if (pi_->flags & lt::peer_info::plaintext_encrypted)
    {
        flags += "e ";
    }

    if (pi_->flags & lt::peer_info::utp_socket)
    {
        flags += "P ";
    }

    if (pi_->source & lt::peer_info::lsd)
    {
        flags += "L ";
    }

    return flags;
}
