#include "peerlistmodel.hpp"

#include <map>

#include <libtorrent/peer_info.hpp>

#include <QIcon>
#include <QImageReader>
#include <QPixmap>

#include "../core/geoip/geoip.hpp"
#include "../core/utils.hpp"
#include "../torrenthandle.hpp"
#include "../translator.hpp"

namespace lt = libtorrent;
using pt::PeerListModel;

/*
This lookup table is highly dependant on the flags.svg file,
but it's fast and works. We need to make sure the country code
maps well to what we recieve from the GeoLite country database.
*/
static const std::map<std::string, QPoint> FlagPositions = 
{
    { "AD", QPoint(10,  10)  }, // Andorra, but the crest is different
    { "AE", QPoint(41,  10)  }, // United Arab Emirates
    { "AG", QPoint(72,  10)  }, // Antigua and Barbuda
    { "AM", QPoint(103, 10)  }, // Armenia
    { "AR", QPoint(134, 10)  }, // Argentia
    { "AT", QPoint(165, 10)  }, // Austria
    { "AU", QPoint(196, 10)  }, // Australia
    { "BE", QPoint(227, 10)  }, // Belgium
    { "BF", QPoint(258, 10)  }, // Burkina Faso
    { "BG", QPoint(289, 10)  }, // Bulgaria
    { "BO", QPoint(320, 10)  }, // Bolivia
    { "BR", QPoint(351, 10)  }, // Brazil
    { "CA", QPoint(382, 10)  }, // Canada
    { "CD", QPoint(413, 10)  }, // Democratic Republic of the Congo
    { "CG", QPoint(444, 10)  }, // Republic of the Congo
    { "CH", QPoint(475, 10)  }, // Schweiz
    { "CL", QPoint(506, 10)  }, // Chile

    { "CM", QPoint(10,  35)  }, // Cameroon
    { "CN", QPoint(41,  35)  }, // China
    { "CO", QPoint(72,  35)  }, // Colombia
    { "CZ", QPoint(103, 35)  }, // Czech Republic
    { "DE", QPoint(134, 35)  }, // Germany
    { "DJ", QPoint(165, 35)  }, // Djibouti
    { "DK", QPoint(196, 35)  }, // Denmark
    { "DZ", QPoint(227, 35)  }, // Algeria
    { "EE", QPoint(258, 35)  }, // Estonia
    { "EG", QPoint(289, 35)  }, // Egypt, but the crest looks different
    { "ES", QPoint(320, 35)  }, // Spain
    { "EU", QPoint(351, 35)  }, // European Union... not a country
    { "FI", QPoint(382, 35)  }, // Finland
    { "FR", QPoint(413, 35)  }, // France
    { "GA", QPoint(444, 35)  }, // Gabon
    { "GB", QPoint(475, 35)  }, // United Kingdom of Great Britain and Northern Ireland
    { "GM", QPoint(506, 35)  }, // Gambia

    { "GT", QPoint(10,  60)  }, // Guatemala
    { "HK", QPoint(41,  60)  }, // Hong Kong
    { "HN", QPoint(72,  60)  }, // Honduras
    { "HT", QPoint(103, 60)  }, // Haiti
    { "HU", QPoint(134, 60)  }, // Hungary
    { "ID", QPoint(165, 60)  }, // Indonesia
    { "IE", QPoint(196, 60)  }, // Ireland
    { "IL", QPoint(227, 60)  }, // Israel
    { "IN", QPoint(258, 60)  }, // India
    { "IQ", QPoint(289, 60)  }, // Iraq
    { "IR", QPoint(320, 60)  }, // Iran
    { "IT", QPoint(351, 60)  }, // Italy
    { "JM", QPoint(382, 60)  }, // Jamaica
    { "JO", QPoint(413, 60)  }, // Jordan
    { "JP", QPoint(444, 60)  }, // Japan
    { "KG", QPoint(475, 60)  }, // Kyrgyzstan
    { "KN", QPoint(506, 60)  }, // Saint Kitts and Nevis

    { "KP", QPoint(10,  85)  }, // North Korea
    { "KR", QPoint(41,  85)  }, // South Korea
    { "KW", QPoint(72,  85)  }, // Kuwait
    { "KZ", QPoint(103, 85)  }, // Kazakhstan
    { "LA", QPoint(134, 85)  }, // Laos
    { "LB", QPoint(165, 85)  }, // Lebanon
    { "LC", QPoint(196, 85)  }, // Saint Lucia
    { "LS", QPoint(227, 85)  }, // Lesotho
    { "LU", QPoint(258, 85)  }, // Luxembourg
    { "LV", QPoint(289, 85)  }, // Latvia
    { "MG", QPoint(320, 85)  }, // Madagascar
    { "MK", QPoint(351, 85)  }, // Macedonia
    { "ML", QPoint(382, 85)  }, // Mali
    { "MM", QPoint(413, 85)  }, // Myanmar
    { "MT", QPoint(444, 85)  }, // Malta
    { "MX", QPoint(475, 85)  }, // Mexico
    { "NA", QPoint(506, 85)  }, // Namibia

    { "NE", QPoint(10,  110) }, // Niger
    { "NG", QPoint(41,  110) }, // Nigeria
    { "NI", QPoint(72,  110) }, // Nicaragua
    { "NL", QPoint(103, 110) }, // Netherlands
    { "NO", QPoint(134, 110) }, // Norway
    { "NZ", QPoint(165, 110) }, // New Zealand
    { "OM", QPoint(196, 110) }, // Oman
    { "PA", QPoint(227, 110) }, // Panama
    { "PE", QPoint(258, 110) }, // Peru, but missing the crest
    { "PG", QPoint(289, 110) }, // Papua New Guinea
    { "PH", QPoint(320, 110) }, // Phillippines
    { "PK", QPoint(351, 110) }, // Pakistan
    { "PL", QPoint(382, 110) }, // Poland
    { "PT", QPoint(413, 110) }, // Portugal
    { "PY", QPoint(444, 110) }, // Paraguay
    { "QA", QPoint(475, 110) }, // Qatar
    { "RO", QPoint(506, 110) }, // Romania

    { "RU", QPoint(10,  135) }, // Russia
    { "RW", QPoint(41,  135) }, // Rwanda
    { "SA", QPoint(72,  135) }, // Saudi Arabia
    { "SE", QPoint(103, 135) }, // Sweden
    { "SG", QPoint(134, 135) }, // Singapore
    { "SL", QPoint(165, 135) }, // Sierra Leone
    { "SN", QPoint(196, 135) }, // Senegal
    { "SO", QPoint(227, 135) }, // Somalia
    { "SV", QPoint(258, 135) }, // El Salvador
    { "TD", QPoint(289, 135) }, // Tchad
    { "TJ", QPoint(320, 135) }, // Tajikistan
    { "TL", QPoint(351, 135) }, // Timor-Leste
    { "TR", QPoint(382, 135) }, // Turkey
    { "TZ", QPoint(413, 135) }, // Tanzania
    { "UA", QPoint(444, 135) }, // Ukraine
    { "US", QPoint(475, 135) }, // USA
    { "VE", QPoint(506, 135) }, // Venezuela

    { "VN", QPoint(10,  150) }, // Vietnam
    { "",   QPoint(41,  150) }, // ---- Unknown (to me)
    { "YE", QPoint(72,  150) }, // Yemen
};

PeerListModel::PeerListModel(pt::GeoIP* geo)
    : m_geo(geo)
{
    QIcon icn(":/flags.svg");
    QPixmap pm = icn.pixmap(537, 185).scaledToWidth(537);
    m_flags = new QPixmap(pm);
}

PeerListModel::~PeerListModel()
{
    delete m_flags;
}

void PeerListModel::clear()
{
    this->beginResetModel();
    m_peers.clear();
    this->endResetModel();
}

void PeerListModel::update(pt::TorrentHandle* torrent)
{
    std::vector<lt::peer_info> peers;
    torrent->getPeerInfo(peers);

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
    case Qt::DecorationRole:
    {
        switch (index.column())
        {
        case Columns::IP:
        {
            std::string code = m_geo->lookupCountryCode(peer.ip.address().to_string());

            if (FlagPositions.find(code) != FlagPositions.end())
            {
                QPoint const& point = FlagPositions.at(code);
                return m_flags->copy(point.x(), point.y(), 21, 15);
            }

            break;
        }
        }
        break;
    }
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

            return QString("%1/s").arg(Utils::toHumanFileSize(peer.payload_down_speed));
        }
        case Columns::UploadRate:
        {
            if (peer.payload_up_speed <= 0)
            {
                return "-";
            }

            return QString("%1/s").arg(Utils::toHumanFileSize(peer.payload_up_speed));
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
