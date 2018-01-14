#include "peersviewmodel.hpp"

#include <libtorrent/torrent_status.hpp>

#include "utils.hpp"

namespace lt = libtorrent;
using pt::PeersViewModel;

void PeersViewModel::Clear()
{
    m_data.clear();
    Reset(0);
}

void PeersViewModel::Update(lt::torrent_status const& ts)
{
    std::vector<lt::peer_info> peers;
    ts.handle.get_peer_info(peers);

    // Remove old data
    for (auto it = m_data.begin(); it != m_data.end();)
    {
        auto f = std::find_if(peers.begin(), peers.end(), [it](lt::peer_info& p) { return p.ip == it->ip; });

        if (f == peers.end())
        {
            it = m_data.erase(it);
            RowDeleted(std::distance(m_data.begin(), it));
        }
        else
        {
            ++it;
        }
    }

    // Add or update new data

    for (auto it = peers.begin(); it != peers.end(); it++)
    {
        auto f = std::find_if(m_data.begin(), m_data.end(), [it](lt::peer_info& p) { return p.ip == it->ip; });

        if (f == m_data.end())
        {
            m_data.push_back(*it);
            RowAppended();
        }
        else
        {
            auto distance = std::distance(m_data.begin(), f);
            m_data.at(distance) = *f;
            RowChanged(distance);
        }
    }
}

unsigned int PeersViewModel::GetColumnCount() const
{
    return Columns::_Max;
}

wxString PeersViewModel::GetColumnType(unsigned int col) const
{
    return "string";
}

void PeersViewModel::GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const
{
    lt::peer_info const& peer = m_data.at(row);

    switch (col)
    {
    case Columns::IP:
        variant = peer.ip.address().to_string();
        break;
    case Columns::Client:
        variant = peer.client;
        break;
    case Columns::Flags:
    {
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

        variant = flags.str();

        break;
    }
    case Columns::DownloadRate:
    {
        if (peer.payload_down_speed <= 0)
        {
            variant = "-";
        }
        else
        {
            variant = wxString::Format(
                "%s/s",
                Utils::ToHumanFileSize(peer.payload_down_speed));
        }

        break;
    }
    case Columns::UploadRate:
    {
        if (peer.payload_up_speed <= 0)
        {
            variant = "-";
        }
        else
        {
            variant = wxString::Format(
                "%s/s",
                Utils::ToHumanFileSize(peer.payload_up_speed));
        }

        break;
    }
    case Columns::Progress:
    {
        variant = static_cast<long>(peer.progress * 100);
        break;
    }
    }
}

bool PeersViewModel::SetValueByRow(const wxVariant &variant, unsigned row, unsigned col)
{
    return false;
}

