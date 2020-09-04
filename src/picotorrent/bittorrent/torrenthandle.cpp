#include "torrenthandle.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include "session.hpp"
#include "torrentstatus.hpp"

namespace lt = libtorrent;
using pt::BitTorrent::TorrentHandle;
using pt::BitTorrent::TorrentStatus;

TorrentStatus::State getTorrentStatusState(lt::torrent_status const& ts)
{
    bool paused = ((ts.flags & lt::torrent_flags::paused)
        && !(ts.flags & lt::torrent_flags::auto_managed));

    bool seeding = (ts.state == lt::torrent_status::state_t::seeding
        || ts.state == lt::torrent_status::state_t::finished);

    bool queued = ((ts.flags & lt::torrent_flags::paused)
        && (ts.flags & lt::torrent_flags::auto_managed));

    bool checking = (ts.state == lt::torrent_status::state_t::checking_files
        || ts.state == lt::torrent_status::state_t::checking_resume_data);

    if (ts.errc)
    {
        return TorrentStatus::State::Error;
    }

    if (paused)
    {
        if (seeding)
        {
            return TorrentStatus::State::UploadingPaused;
        }

        return TorrentStatus::State::DownloadingPaused;
    }
    else
    {
        if (queued && !checking)
        {
            return seeding
                ? TorrentStatus::State::UploadingQueued
                : TorrentStatus::State::DownloadingQueued;
        }
        else
        {
            switch (ts.state)
            {
            case lt::torrent_status::state_t::finished:
            case lt::torrent_status::state_t::seeding:
                return paused
                    ? TorrentStatus::State::UploadingPaused
                    : TorrentStatus::State::Uploading;

            case lt::torrent_status::state_t::checking_resume_data:
                return TorrentStatus::State::CheckingResumeData;

            case lt::torrent_status::state_t::checking_files:
                return TorrentStatus::State::DownloadingChecking;

            case lt::torrent_status::state_t::downloading_metadata:
                return TorrentStatus::State::DownloadingMetadata;

            case lt::torrent_status::state_t::downloading:
                return TorrentStatus::State::Downloading;
            }
        }
    }

    return TorrentStatus::State::Unknown;
}

TorrentHandle::TorrentHandle(pt::BitTorrent::Session* session, lt::torrent_handle const& th)
    : m_session(session)
{
    m_th = std::make_unique<lt::torrent_handle>(th);
    m_ts = std::make_unique<lt::torrent_status>(th.status());
}

TorrentHandle::~TorrentHandle()
{
}

void TorrentHandle::AddTracker(lt::announce_entry const& entry)
{
    m_th->add_tracker(entry);
}

void TorrentHandle::FileProgress(std::vector<std::int64_t>& progress, int flags) const
{
    m_th->file_progress(
        progress,
        flags >= 1
            ? lt::torrent_handle::piece_granularity
            : lt::file_progress_flags_t{});
}

void TorrentHandle::ForceReannounce()
{
    m_th->force_reannounce();
}

void TorrentHandle::ForceReannounce(int seconds, int trackerIndex)
{
    m_th->force_reannounce(seconds, trackerIndex);
}

void TorrentHandle::ForceRecheck()
{
    if (Status().paused)
    {
        m_session->PauseAfterRecheck(this);
        Resume();
    }

    m_th->force_recheck();
}

std::vector<lt::download_priority_t> TorrentHandle::GetFilePriorities() const
{
    return m_th->get_file_priorities();
}

void TorrentHandle::GetPeerInfo(std::vector<lt::peer_info>& peers) const
{
    m_th->get_peer_info(peers);
}

lt::info_hash_t TorrentHandle::InfoHash()
{
    return m_th->info_hashes();
}

bool TorrentHandle::IsSequentialDownload()
{
    return (m_th->flags() & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download;
}

bool TorrentHandle::IsValid()
{
    return m_th->is_valid();
}

void TorrentHandle::MoveStorage(std::string const& newPath)
{
    m_th->move_storage(newPath);
}

void TorrentHandle::Pause()
{
    m_th->unset_flags(lt::torrent_flags::auto_managed);
    m_th->pause(lt::torrent_handle::graceful_pause);
}

void TorrentHandle::QueueUp()
{
    m_th->queue_position_up();
}

void TorrentHandle::QueueDown()
{
    m_th->queue_position_down();
}

void TorrentHandle::QueueTop()
{
    m_th->queue_position_top();
}

void TorrentHandle::QueueBottom()
{
    m_th->queue_position_bottom();
}

void TorrentHandle::ReplaceTrackers(std::vector<lt::announce_entry> const& trackers)
{
    m_th->replace_trackers(trackers);
}

void TorrentHandle::Remove()
{
    m_session->RemoveTorrent(this);
}

void TorrentHandle::RemoveFiles()
{
    m_session->RemoveTorrent(this, lt::session_handle::delete_files);
}

void TorrentHandle::Resume()
{
    m_th->set_flags(lt::torrent_flags::auto_managed);
    m_th->clear_error();
    m_th->resume();
}

void TorrentHandle::ResumeForce()
{
    m_th->unset_flags(lt::torrent_flags::auto_managed);
    m_th->clear_error();
    m_th->resume();
}

void TorrentHandle::ScrapeTracker(int trackerIndex)
{
    m_th->scrape_tracker(trackerIndex);
}

void TorrentHandle::SetFilePriorities(std::vector<lt::download_priority_t> priorities)
{
    m_th->prioritize_files(priorities);
}

void TorrentHandle::SetFilePriority(lt::file_index_t index, lt::download_priority_t priority)
{
    m_th->file_priority(index, priority);
}

void TorrentHandle::SetSequentialDownload(bool seq)
{
    if (seq)
    {
        m_th->set_flags(lt::torrent_flags::sequential_download);
    }
    else
    {
        m_th->unset_flags(lt::torrent_flags::sequential_download);
    }
}

TorrentStatus TorrentHandle::Status()
{
    std::stringstream hash;

    if (m_ts->info_hashes.has_v2())
    {
        hash << m_ts->info_hashes.v2;
    }
    else
    {
        hash << m_ts->info_hashes.v1;
    }

    if (!m_th->is_valid())
    {
        TorrentStatus ts;
        ts.infoHash = hash.str();
        return ts;
    }

    auto eta = std::chrono::seconds(0);

    if ((m_ts->total_wanted - m_ts->total_wanted_done > 0) && m_ts->download_payload_rate > 0)
    {
        eta = std::chrono::seconds((m_ts->total_wanted - m_ts->total_wanted_done) / m_ts->download_payload_rate);
    }

    float ratio = 0;

    if (m_ts->all_time_download > 0)
    {
        ratio = static_cast<float>(m_ts->all_time_upload) / static_cast<float>(m_ts->all_time_download);
    }

    std::string error;
    std::string error_details;

    if (m_ts->errc)
    {
        error = m_ts->errc.message();

        // If we have an error in a file, get file info
        auto ti = m_ts->torrent_file.lock();

        if (m_ts->error_file >= lt::file_index_t{ 0 } && ti)
        {
            error_details = ti->files().file_path(m_ts->error_file);
        }
    }

    TorrentStatus ts;
    ts.addedOn              = wxDateTime(m_ts->added_time);
    ts.allTimeDownload      = m_ts->all_time_download;
    ts.allTimeUpload        = m_ts->all_time_upload;
    ts.availability         = m_ts->distributed_copies;
    ts.completedOn          = m_ts->completed_time > 0 ? wxDateTime(m_ts->completed_time) : wxDateTime();
    ts.downloadPayloadRate  = m_ts->download_payload_rate;
    ts.error                = error;
    ts.errorDetails         = error_details;
    ts.eta                  = eta;
    ts.forced               = (!(m_ts->flags & lt::torrent_flags::paused) && !(m_ts->flags & lt::torrent_flags::auto_managed));
    ts.infoHash             = hash.str();
    ts.lastDownload         = m_ts->last_download.time_since_epoch().count() > 0 ? std::chrono::seconds(lt::total_seconds(lt::clock_type::now() - m_ts->last_download)) : std::chrono::seconds(-1);
    ts.lastUpload           = m_ts->last_upload.time_since_epoch().count() > 0 ? std::chrono::seconds(lt::total_seconds(lt::clock_type::now() - m_ts->last_upload)) : std::chrono::seconds(-1);
    ts.name                 = m_ts->name.empty() ? ts.infoHash : m_ts->name;
    ts.paused               = (m_th->flags() & lt::torrent_flags::paused) == lt::torrent_flags::paused;
    ts.peersCurrent         = m_ts->num_peers - m_ts->num_seeds;
    ts.peersTotal           = m_ts->list_peers - m_ts->list_seeds;
    ts.pieces               = m_ts->pieces;
    ts.progress             = m_ts->progress;
    ts.queuePosition        = static_cast<int>(m_ts->queue_position);
    ts.ratio                = ratio;
    ts.savePath             = m_ts->save_path;
    ts.seedsCurrent         = m_ts->num_seeds;
    ts.seedsTotal           = m_ts->list_seeds;
    ts.state                = getTorrentStatusState(*m_ts.get());
    ts.torrentFile          = m_ts->torrent_file;
    ts.totalWanted          = m_ts->total_wanted;
    ts.totalWantedRemaining = m_ts->total_wanted - m_ts->total_wanted_done;
    ts.uploadPayloadRate    = m_ts->upload_payload_rate;
    return ts;
}

std::vector<lt::announce_entry> TorrentHandle::Trackers() const
{
    return m_th->trackers();
}

void TorrentHandle::UpdateStatus(lt::torrent_status const& ts)
{
    m_ts = std::make_unique<lt::torrent_status>(ts);
}

lt::torrent_handle& TorrentHandle::WrappedHandle()
{
    return *m_th.get();
}
