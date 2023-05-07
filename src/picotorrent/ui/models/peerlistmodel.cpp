#include "peerlistmodel.hpp"

#include <fmt/format.h>
#include <fmt/xchar.h>
#include <libtorrent/peer_info.hpp>

#include "../../core/utils.hpp"
#include "../translator.hpp"

namespace lt = libtorrent;
using pt::UI::Models::PeerListModel;

PeerListModel::PeerListModel()
{
}

PeerListModel::~PeerListModel()
{
}

void PeerListModel::ResetPeers()
{
    m_data.clear();
    Reset(0);
}

void PeerListModel::Update(std::vector<lt::peer_info> const& peers)
{
    // Remove old data
    for (auto it = m_data.begin(); it != m_data.end();)
    {
        auto f = std::find_if(
            peers.begin(),
            peers.end(),
            [it](lt::peer_info const& p)
            {
                return p.ip == it->ip;
            });

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
            m_data.at(distance) = *it;
            RowChanged(distance);
        }
    }
}

unsigned int PeerListModel::GetColumnCount() const
{
    return Column::_Max;
}

wxString PeerListModel::GetColumnType(unsigned int) const
{
    return "string";
}

void PeerListModel::GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const
{
    lt::peer_info const& peer = m_data.at(row);

    switch (col)
    {
    case Column::IP:
        variant = peer.ip.address().to_string();
        break;
    case Column::Client:
        variant = wxString::FromUTF8(peer.client);
        break;
    case Column::Flags:
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

        if (!(peer.flags & lt::peer_info::outgoing_connection))
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
    case Column::DownloadRate:
    {
        if (peer.payload_down_speed <= 0)
        {
            variant = "-";
        }
        else
        {
            variant = fmt::format(
                i18n("per_second_format"),
                Utils::toHumanFileSize(peer.payload_down_speed));
        }

        break;
    }
    case Column::UploadRate:
    {
        if (peer.payload_up_speed <= 0)
        {
            variant = "-";
        }
        else
        {
            variant = fmt::format(
                i18n("per_second_format"),
                Utils::toHumanFileSize(peer.payload_up_speed));
        }

        break;
    }
    case Column::Progress:
    {
        variant = static_cast<long>(peer.progress * 100);
        break;
    }
    }
}

bool PeerListModel::SetValueByRow(const wxVariant&, unsigned int, unsigned int)
{
    return false;
}
