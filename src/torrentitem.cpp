#include "torrentitem.h"

#include <boost/shared_ptr.hpp>
#include <libtorrent/aux_/escape_string.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

using pico::TorrentItem;

TorrentItem::TorrentItem(libtorrent::torrent_status const& status)
{
    downloadRate_ = status.download_payload_rate;
    uploadRate_ = status.upload_payload_rate;
    name_ = libtorrent::convert_to_wstring(status.name);
    progress_ = status.progress;
    queuePosition_ = status.queue_position;
    size_ = -1;
    
    if (boost::shared_ptr<const libtorrent::torrent_info> ti = status.torrent_file.lock())
    {
        size_ = ti->total_size();
    }
    
    status_ = status.state;
}

int const TorrentItem::GetDownloadRate()
{
    return downloadRate_;
}

int const TorrentItem::GetUploadRate()
{
    return uploadRate_;
}

std::wstring const& TorrentItem::GetName()
{
    return name_;
}

float const TorrentItem::GetProgress()
{
    return progress_;
}

int const TorrentItem::GetQueuePosition()
{
    return queuePosition_;
}

int64_t const TorrentItem::GetSize()
{
    return size_;
}

int const TorrentItem::GetStatus()
{
    return status_;
}

void TorrentItem::SetDownloadRate(int rate)
{
    downloadRate_ = rate;
}

void TorrentItem::SetUploadRate(int rate)
{
    uploadRate_ = rate;
}

void TorrentItem::SetName(std::wstring const& name)
{
    name_ = name;
}

void TorrentItem::SetProgress(float progress)
{
    progress_ = progress;
}

void TorrentItem::SetQueuePosition(int queuePosition)
{
    queuePosition_ = queuePosition;
}

void TorrentItem::SetSize(size_t size)
{
    size_ = size;
}

void TorrentItem::SetStatus(int status)
{
    status_ = status;
}
