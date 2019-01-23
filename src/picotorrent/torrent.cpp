#include "torrent.hpp"

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include <picotorrent/core/utils.hpp>

namespace lt = libtorrent;
using pt::Torrent;

Torrent::Torrent(QObject* parent, lt::torrent_handle const& th)
    : QObject(parent)
{
    m_th = std::make_unique<lt::torrent_handle>(th);
    m_ts = std::make_unique<lt::torrent_status>(th.status());
}

Torrent::~Torrent()
{
}

QDateTime Torrent::addedOn()
{
    return QDateTime::fromSecsSinceEpoch(m_ts->added_time);
}

float Torrent::availability()
{
    return m_ts->distributed_copies;
}

QString Torrent::availabilityFormatted()
{
    QString str;
    str.sprintf("%.3f", availability());
    return str;
}

QDateTime Torrent::completedOn()
{
    if (m_ts->completed_time > 0)
    {
        return QDateTime::fromSecsSinceEpoch(m_ts->completed_time);
    }

    return QDateTime();
}

int Torrent::downloadRate()
{
    return m_ts->download_payload_rate;
}

QString Torrent::downloadRateFormatted()
{
    return QString("%1/s").arg(Utils::toHumanFileSize(downloadRate()));
}

std::chrono::seconds Torrent::eta()
{
    int64_t remainingBytes = m_ts->total_wanted - m_ts->total_wanted_done;

    if (remainingBytes > 0 && m_ts->download_payload_rate > 0)
    {
        return std::chrono::seconds(remainingBytes / m_ts->download_payload_rate);
    }

    return std::chrono::seconds(0);
}

QString Torrent::etaFormatted()
{
    auto secs = eta();

    std::chrono::hours hours_left = std::chrono::duration_cast<std::chrono::hours>(secs);
    std::chrono::minutes min_left = std::chrono::duration_cast<std::chrono::minutes>(secs - hours_left);
    std::chrono::seconds sec_left = std::chrono::duration_cast<std::chrono::seconds>(secs - hours_left - min_left);

    return QString("%1h %2m %3s").arg(
        QString::number(hours_left.count()),
        QString::number(min_left.count()),
        QString::number(sec_left.count()));
}

QString Torrent::infoHash()
{
    return "Infohash";
}

QString Torrent::name()
{
    if (m_ts->name.empty())
    {
        return infoHash();
    }

    return QString::fromStdString(m_ts->name);
}

float Torrent::progress()
{
    return m_ts->progress;
}

int Torrent::peersCurrent()
{
    return m_ts->num_peers - m_ts->num_seeds;
}

int Torrent::peersTotal()
{
    return m_ts->list_peers - m_ts->list_seeds;
}

int Torrent::queuePosition()
{
    return static_cast<int>(m_ts->queue_position);
}

float Torrent::ratio()
{
    if (m_ts->all_time_download > 0)
    {
        return static_cast<float>(m_ts->all_time_upload) / static_cast<float>(m_ts->all_time_download);
    }

    return 0;
}

QString Torrent::ratioFormatted()
{
    QString str;
    str.sprintf("%.3f", ratio());
    return str;
}

int Torrent::seedsCurrent()
{
    return m_ts->num_seeds;
}

int Torrent::seedsTotal()
{
    return m_ts->list_seeds;
}

QString Torrent::sizeTotalFormatted()
{
    return QString::fromStdWString(Utils::toHumanFileSize(m_ts->total_wanted));
}

int Torrent::uploadRate()
{
    return m_ts->upload_payload_rate;
}

QString Torrent::uploadRateFormatted()
{
    return QString("%1/s").arg(Utils::toHumanFileSize(uploadRate()));
}

void Torrent::updateStatus(lt::torrent_status const& ts)
{
    m_ts = std::make_unique<lt::torrent_status>(ts);
}
