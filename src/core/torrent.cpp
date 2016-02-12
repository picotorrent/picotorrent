#include <picotorrent/core/torrent.hpp>

#include <picotorrent/core/hash.hpp>
#include <picotorrent/core/peer.hpp>
#include <picotorrent/core/torrent_info.hpp>
#include <picotorrent/core/torrent_state.hpp>
#include <picotorrent/core/tracker.hpp>
#include <picotorrent/core/tracker_status.hpp>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/announce_entry.hpp>
#include <libtorrent/peer_info.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

namespace lt = libtorrent;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;
using picotorrent::core::hash;
using picotorrent::core::peer;
using picotorrent::core::torrent;
using picotorrent::core::torrent_info;
using picotorrent::core::torrent_state;
using picotorrent::core::tracker;
using picotorrent::core::tracker_status;

torrent::torrent(const lt::torrent_status &st)
    : status_(std::make_shared<lt::torrent_status>(st)),
    state_(torrent_state::state_t::unknown)
{
}

torrent::~torrent()
{
}

int torrent::download_limit() const
{
    return status_->handle.download_limit();
}

int torrent::download_rate()
{
    return status_->download_payload_rate;
}

int torrent::eta() const
{
	if (is_paused())
	{
		return -1;
	}

	int64_t remaining = status_->total_wanted - status_->total_wanted_done;

	if (remaining > 0 && status_->download_payload_rate > 0)
	{
        return (int)(remaining / status_->download_payload_rate);
	}

	return -1;
}

std::vector<int> torrent::file_priorities() const
{
    return status_->handle.file_priorities();
}

void torrent::file_priority(int file_index, int priority)
{
    status_->handle.file_priority(file_index, priority);
}

void torrent::file_progress(std::vector<int64_t> &progress, int flags) const
{
    status_->handle.file_progress(progress, flags);
}

std::vector<peer> torrent::get_peers()
{
    std::vector<lt::peer_info> peers;
    status_->handle.get_peer_info(peers);
    return std::vector<peer>(peers.begin(), peers.end());
}

std::vector<tracker> torrent::get_trackers()
{
    std::vector<lt::announce_entry> trackers = status_->handle.trackers();
    return std::vector<tracker>(trackers.begin(), trackers.end());
}

tracker_status& torrent::get_tracker_status(const std::string &url)
{
    return tracker_status_[url];
}

bool torrent::has_error() const
{
    return status_->paused && status_->errc;
}

std::shared_ptr<hash> torrent::info_hash()
{
    return std::make_shared<hash>(status_->info_hash);
}

bool torrent::is_checking() const
{
    return status_->state == lt::torrent_status::state_t::checking_files
        || status_->state == lt::torrent_status::state_t::checking_resume_data;
}

bool torrent::is_forced() const
{
    return !status_->paused && !status_->auto_managed;
}

bool torrent::is_paused() const
{
    return status_->paused && !status_->auto_managed;
}

bool torrent::is_queued() const
{
    return status_->paused && status_->auto_managed;
}

bool torrent::is_seeding() const
{
    return status_->state == lt::torrent_status::state_t::finished
        || status_->state == lt::torrent_status::state_t::seeding;
}

bool torrent::is_valid()
{
    return status_->handle.is_valid();
}

int torrent::max_connections() const
{
    return status_->handle.max_connections();
}

int torrent::max_uploads() const
{
    return status_->handle.max_uploads();
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
    if (is_paused())
    {
        return;
    }

    status_->handle.auto_managed(false);
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

void torrent::resume(bool force)
{
    if (has_error())
    {
        status_->handle.clear_error();
    }

    status_->handle.set_upload_mode(false);
    status_->handle.auto_managed(!force);
    status_->handle.resume();
}

std::string torrent::save_path() const
{
    return status_->save_path;
}

void torrent::set_download_limit(int limit)
{
    status_->handle.set_download_limit(limit);
}

void torrent::set_max_connections(int limit)
{
    status_->handle.set_max_connections(limit);
}

void torrent::set_max_uploads(int limit)
{
    status_->handle.set_max_uploads(limit);
}

void torrent::set_sequential_download(bool val)
{
    status_->handle.set_sequential_download(val);
}

void torrent::set_upload_limit(int limit)
{
    status_->handle.set_upload_limit(limit);
}

bool torrent::sequential_download() const
{
    return status_->sequential_download;
}

int64_t torrent::size()
{
    if (boost::shared_ptr<const lt::torrent_info> info = status_->torrent_file.lock())
    {
        return info->total_size();
    }

    return -1;
}

torrent_state torrent::state()
{
    return state_;
}

std::shared_ptr<const torrent_info> torrent::torrent_info() const
{
    if (!status_->handle.torrent_file())
    {
        return nullptr;
    }

    return std::make_shared<core::torrent_info>(*status_->handle.torrent_file());
}

uint64_t torrent::total_wanted()
{
    return status_->total_wanted;
}

uint64_t torrent::total_wanted_done()
{
    return status_->total_wanted_done;
}

int torrent::upload_limit() const
{
    return status_->handle.upload_limit();
}

int torrent::upload_rate()
{
    return status_->upload_payload_rate;
}

signal_connector<void, void>& torrent::on_updated()
{
    return updated_signal_;
}

void torrent::handle(const lt::scrape_reply_alert &alert)
{
    tracker_status &ts = tracker_status_[alert.tracker_url()];
    ts.scrape_complete = alert.complete;
    ts.scrape_incomplete = alert.incomplete;
}

void torrent::handle(const lt::tracker_reply_alert &alert)
{
    tracker_status &ts = tracker_status_[alert.tracker_url()];
    ts.num_peers = alert.num_peers;
}

void torrent::update(std::unique_ptr<lt::torrent_status> status)
{
    status_ = std::move(status);
    update_state();
    updated_signal_.emit();
}

void torrent::update_state()
{
    if (is_paused())
    {
        if (has_error())
        {
            state_ = torrent_state::state_t::error;
        }
        else
        {
            if (is_seeding())
            {
                state_ = torrent_state::state_t::uploading_paused;
            }
            else
            {
                state_ = torrent_state::state_t::downloading_paused;
            }
        }
    }
    else
    {
        if (is_queued() && !is_checking())
        {
            if (is_seeding())
            {
                state_ = torrent_state::state_t::uploading_queued;
            }
            else
            {
                state_ = torrent_state::state_t::downloading_queued;
            }
        }
        else
        {
            switch (status_->state)
            {
            case lt::torrent_status::state_t::finished:
            case lt::torrent_status::state_t::seeding:
            {
                if (is_forced())
                {
                    state_ = torrent_state::state_t::uploading_forced;
                }
                else
                {
                    if (status_->upload_payload_rate > 0)
                    {
                        state_ = torrent_state::state_t::uploading;
                    }
                    else
                    {
                        state_ = torrent_state::state_t::uploading_stalled;
                    }
                }
                break;
            }

            case lt::torrent_status::state_t::checking_resume_data:
            {
                state_ = torrent_state::state_t::checking_resume_data;
                break;
            }

            case lt::torrent_status::state_t::checking_files:
            {
                state_ = torrent_state::state_t::downloading_checking;
                break;
            }

            case lt::torrent_status::state_t::downloading_metadata:
            {
                state_ = torrent_state::state_t::downloading_metadata;
                break;
            }

            case lt::torrent_status::state_t::downloading:
            {
                if (is_forced())
                {
                    state_ = torrent_state::state_t::downloading_forced;
                }
                else
                {
                    if (status_->download_payload_rate > 0)
                    {
                        state_ = torrent_state::state_t::downloading;
                    }
                    else
                    {
                        state_ = torrent_state::state_t::downloading_stalled;
                    }
                }
                break;
            }
            }
        }
    }
}
