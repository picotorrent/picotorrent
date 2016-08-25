#include "Peer.hpp"

#include <libtorrent/peer_info.hpp>
#include <libtorrent/sha1_hash.hpp>

#include <sstream>

#include "../StringUtils.hpp"

namespace lt = libtorrent;
using Models::Peer;

Peer::Peer(const libtorrent::peer_info& info)
    : m_info(std::make_unique<lt::peer_info>(info))
{
}

Peer::Peer(const Peer& other)
    : m_info(std::make_unique<lt::peer_info>(*other.m_info.get()))
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
    return m_info->pid == other.m_info->pid;
}

bool Peer::operator!=(const Peer& other)
{
    return !(*this == other);
}

std::wstring Peer::endpoint()
{
    std::stringstream ss;
    ss << m_info->ip;
    return TWS(ss.str());
}
