#include <picotorrent/core/torrent.hpp>

#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <picotorrent/core/hash.hpp>

namespace lt = libtorrent;
using picotorrent::core::hash;
using picotorrent::core::torrent;

torrent::torrent(const lt::torrent_status &st)
    : status_(std::make_unique<lt::torrent_status>(st))
{
}

int torrent::download_rate()
{
    return status_->download_payload_rate;
}

std::shared_ptr<hash> torrent::info_hash()
{
    return std::make_shared<hash>(status_->info_hash);
}

bool torrent::is_paused() const
{
    return status_->paused;
}

bool torrent::is_valid()
{
    return status_->handle.is_valid();
}

void torrent::move_storage(const std::string &path)
{
    status_->handle.move_storage(path);
}

std::string& torrent::name() const
{
    return status_->name;
}

void torrent::pause()
{
    if (!is_valid())
    {
        // TODO(log)
        return;
    }

    status_->handle.pause();
}

float torrent::progress() const
{
    return status_->progress;
}

int torrent::queue_position()
{
    return status_->queue_position;
}

void torrent::resume()
{
    if (!is_valid())
    {
        // TODO(log)
        return;
    }

    status_->handle.resume();
}

std::string torrent::save_path() const
{
    return status_->save_path;
}

int64_t torrent::size()
{
    if (boost::shared_ptr<const lt::torrent_info> info = status_->torrent_file.lock())
    {
        return info->total_size();
    }

    return -1;
}

int torrent::upload_rate()
{
    return status_->upload_payload_rate;
}
