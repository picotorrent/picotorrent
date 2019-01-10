#include "peerlistmodel.hpp"

#include <libtorrent/peer_info.hpp>
#include <libtorrent/torrent_handle.hpp>

#include "translator.hpp"
#include "utils.hpp"

namespace lt = libtorrent;
using pt::PeerListModel;

PeerListModel::PeerListModel()
{
}

PeerListModel::~PeerListModel()
{
}

void PeerListModel::update(lt::torrent_handle const& th)
{
    std::vector<lt::peer_info> peers;
    th.get_peer_info(peers);

    // Remove old data
    for (auto it = m_peers.begin(); it != m_peers.end();)
    {
        auto f = std::find_if(peers.begin(), peers.end(), [it](lt::peer_info& p) { return p.ip == it->ip; });

        if (f == peers.end())
        {
            auto distance = std::distance(m_peers.begin(), it);

            this->beginRemoveRows(QModelIndex(), distance, distance);
            it = m_peers.erase(it);
            this->endRemoveRows();
        }
        else
        {
            ++it;
        }
    }

    // Add or update new data
    for (auto it = peers.begin(); it != peers.end(); it++)
    {
        auto f = std::find_if(m_peers.begin(), m_peers.end(), [it](lt::peer_info& p) { return p.ip == it->ip; });

        if (f == m_peers.end())
        {
            int nextIndex = static_cast<int>(m_peers.size());

            this->beginInsertRows(QModelIndex(), nextIndex, nextIndex);
            m_peers.push_back(*it);
            this->endInsertRows();
        }
        else
        {
            auto distance = std::distance(m_peers.begin(), f);
            m_peers.at(distance) = *it;

            emit dataChanged(
                index(distance, 0),
                index(distance, Columns::_Max - 1));
        }
    }
}

int PeerListModel::columnCount(const QModelIndex&) const
{
    return Columns::_Max;
}

QVariant PeerListModel::data(QModelIndex const& index, int role) const
{
    lt::peer_info const& peer = m_peers.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case Columns::IP:
            return QString::fromStdString(peer.ip.address().to_string());

        case Columns::Client:
            return QString::fromStdString(peer.client);

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

            return QString::fromStdString(flags.str());
        }
        case Columns::DownloadRate:
        {
            if (peer.payload_down_speed <= 0)
            {
                return "-";
            }

            return QString("%1/s").arg(Utils::ToHumanFileSize(peer.payload_down_speed));
        }
        case Columns::UploadRate:
        {
            if (peer.payload_up_speed <= 0)
            {
                return "-";
            }

            return QString("%1/s").arg(Utils::ToHumanFileSize(peer.payload_up_speed));
        }
        case Columns::Progress:
        {
            return peer.progress;
        }
        }

        break;
    }
    }

    return QVariant();
}

QVariant PeerListModel::headerData(int section, Qt::Orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (section)
        {
        case Columns::IP:
            return i18n("ip");

        case Columns::Client:
            return i18n("client");

        case Columns::Flags:
            return i18n("flags");

        case Columns::DownloadRate:
            return i18n("dl");

        case Columns::UploadRate:
            return i18n("ul");

        case Columns::Progress:
            return i18n("progress");
        }

        break;
    }
    }

    return QVariant();
}

QModelIndex PeerListModel::parent(const QModelIndex&)
{
    return QModelIndex();
}

int PeerListModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(m_peers.size());
}
