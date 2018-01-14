#include "peersviewmodel.hpp"

#include <libtorrent/torrent_status.hpp>

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
    return -1;
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
    case 0:
        variant = peer.ip.address().to_string();
        break;
    case 1:
        variant = peer.client;
        break;
    }
}

bool PeersViewModel::SetValueByRow(const wxVariant &variant, unsigned row, unsigned col)
{
    return false;
}

