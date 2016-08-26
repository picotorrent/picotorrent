#include "Peer.hpp"

#include <libtorrent/peer_info.hpp>
#include <libtorrent/sha1_hash.hpp>

#include <sstream>

#include "../StringUtils.hpp"

namespace lt = libtorrent;
using Models::Peer;

Peer::Peer(const std::wstring& endpoint)
    : m_endpoint(endpoint)
{
}

Peer::Peer(const std::wstring& endpoint, const libtorrent::peer_info& info)
    : m_endpoint(endpoint),
    m_info(std::make_unique<lt::peer_info>(info))
{
}

Peer::Peer(const Peer& other)
    : m_endpoint(other.m_endpoint),
    m_info(std::make_unique<lt::peer_info>(*other.m_info.get()))
{
}

Peer::~Peer()
{
}

Peer& Peer::operator=(Peer other)
{
    swap(*this, other);
    return *this;
}

bool Peer::operator==(const Peer& other)
{
    return m_endpoint == other.m_endpoint;
}

bool Peer::operator!=(const Peer& other)
{
    return !(*this == other);
}

std::wstring Peer::GetEndpoint()
{
    return m_endpoint;
}

std::wstring Peer::GetClient()
{
    return TWS(m_info->client);
}

std::wstring Peer::GetFlags()
{
    std::stringstream flags;

    if (m_info->flags & lt::peer_info::interesting)
    {
        if (m_info->flags & lt::peer_info::remote_choked)
        {
            flags << "d ";
        }
        else
        {
            flags << "D ";
        }
    }

    if (m_info->flags & lt::peer_info::remote_interested)
    {
        if (m_info->flags & lt::peer_info::choked)
        {
            flags << "u ";
        }
        else
        {
            flags << "U ";
        }
    }

    if (m_info->flags & lt::peer_info::optimistic_unchoke)
    {
        flags << "O ";
    }

    if (m_info->flags & lt::peer_info::snubbed)
    {
        flags << "S ";
    }

    if (m_info->flags & lt::peer_info::local_connection)
    {
        flags << "I ";
    }

    if (!(m_info->flags & lt::peer_info::remote_choked) && !(m_info->flags & lt::peer_info::interesting))
    {
        flags << "K ";
    }

    if (!(m_info->flags & lt::peer_info::choked) && !(m_info->flags & lt::peer_info::remote_interested))
    {
        flags << "? ";
    }

    if (m_info->source & lt::peer_info::pex)
    {
        flags << "X ";
    }

    if (m_info->source & lt::peer_info::dht)
    {
        flags << "H ";
    }

    if (m_info->flags & lt::peer_info::rc4_encrypted)
    {
        flags << "E ";
    }

    if (m_info->flags & lt::peer_info::plaintext_encrypted)
    {
        flags << "e ";
    }

    if (m_info->flags & lt::peer_info::utp_socket)
    {
        flags << "P ";
    }

    if (m_info->source & lt::peer_info::lsd)
    {
        flags << "L ";
    }

    return TWS(flags.str());
}

int Peer::GetDownloadRate()
{
    return m_info->payload_down_speed;
}

int Peer::GetUploadRate()
{
    return m_info->payload_up_speed;
}
