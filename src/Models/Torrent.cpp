#include "Torrent.hpp"

#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

#include "../StringUtils.hpp"

namespace lt = libtorrent;
using Models::Torrent;

Torrent::Torrent(const lt::sha1_hash& hash)
    : m_hash(std::make_unique<lt::sha1_hash>(hash))
{
}

Torrent::Torrent(const lt::sha1_hash& hash, const lt::torrent_status& ts)
    : m_hash(std::make_unique<lt::sha1_hash>(hash)),
    m_status(std::make_unique<lt::torrent_status>(ts))
{
}

Torrent::Torrent(const Torrent& other)
    : m_hash(std::make_unique<lt::sha1_hash>(*other.m_hash.get())),
    m_status(std::make_unique<lt::torrent_status>(*other.m_status.get()))
{
}

Torrent::~Torrent()
{
}

Torrent& Torrent::operator=(Torrent other)
{
    swap(*this, other);
    return *this;
}

bool Torrent::operator==(const Torrent& other)
{
    return *m_hash == *other.m_hash;
}

bool Torrent::operator!=(const Torrent& other)
{
    return !(*this == other);
}

lt::sha1_hash Torrent::infoHash() const
{
    return *m_hash;
    //return TWS(lt::to_hex(m_status->info_hash.to_string()));
}

std::wstring Torrent::name() const
{
    return TWS(m_status->name);
}

int Torrent::queuePosition() const
{
    return m_status->queue_position;
}

int64_t Torrent::size() const
{
    return m_status->handle.torrent_file()->total_size();
}

int Torrent::status() const
{
    return m_status->state;
}

float Torrent::progress() const
{
    return m_status->progress;
}

int Torrent::eta() const
{
    int eta = -1;

    if (!isPaused())
    {
        int64_t remaining_bytes = m_status->total_wanted - m_status->total_wanted_done;
        if (remaining_bytes > 0 && downloadRate() > 0)
        {
            eta = (int)(remaining_bytes / downloadRate());
        }
    }

    return eta;
}

int Torrent::downloadRate() const
{
    return m_status->download_payload_rate;
}

int Torrent::uploadRate() const
{
    return m_status->upload_payload_rate;
}

int Torrent::seedsConnected() const
{
    return m_status->num_seeds;
}

int Torrent::seedsTotal() const
{
    return m_status->list_seeds;
}

int Torrent::peersConnected() const
{
    return m_status->num_peers - m_status->num_seeds;
}

int Torrent::peersTotal() const
{
    return m_status->list_peers - m_status->list_seeds;
}

float Torrent::shareRatio() const
{
    int64_t ul = m_status->all_time_upload;
    int64_t dl = m_status->all_time_download;
    float ratio = 0;

    if (dl > 0) { ratio = (float)ul / (float)dl; }

    return ratio;
}

bool Torrent::isPaused() const
{
    return m_status->paused && !m_status->auto_managed;
}

std::wstring Torrent::savePath() const
{
    return TWS(m_status->save_path);
}
