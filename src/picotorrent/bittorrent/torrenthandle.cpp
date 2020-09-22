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
    m_status = Update(th.status());
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
    return *m_status.get();
}

std::vector<lt::announce_entry> TorrentHandle::Trackers() const
{
    return m_th->trackers();
}

void TorrentHandle::ClearLabel()
{
    m_labelId = -1;
    m_session->UpdateTorrentLabel(this);
}

int TorrentHandle::Label()
{
    return m_labelId;
}

void TorrentHandle::BuildStatus(libtorrent::torrent_status const& ts)
{
    m_status = Update(ts);
}

void TorrentHandle::SetLabel(int id)
{
    m_labelId = id;
    m_session->UpdateTorrentLabel(this);
}

void TorrentHandle::SetLabelMuted(int id)
{
    m_labelId = id;
}

std::unique_ptr<TorrentStatus> TorrentHandle::Update(lt::torrent_status const& ts)
{
    std::stringstream hash;

    if (ts.info_hashes.has_v2())
    {
        hash << ts.info_hashes.v2;
    }
    else
    {
        hash << ts.info_hashes.v1;
    }

    if (!m_th->is_valid())
    {
        TorrentStatus ts;
        ts.infoHash = hash.str();
        return std::make_unique<TorrentStatus>(ts);
    }

    auto eta = std::chrono::seconds(0);

    if ((ts.total_wanted - ts.total_wanted_done > 0) && ts.download_payload_rate > 0)
    {
        eta = std::chrono::seconds((ts.total_wanted - ts.total_wanted_done) / ts.download_payload_rate);
    }

    float ratio = 0;

    if (ts.all_time_download > 0)
    {
        ratio = static_cast<float>(ts.all_time_upload) / static_cast<float>(ts.all_time_download);
    }

    std::string error;
    std::string error_details;

    if (ts.errc)
    {
        error = ts.errc.message();

        // If we have an error in a file, get file info
        auto ti = ts.torrent_file.lock();

        if (ts.error_file >= lt::file_index_t{ 0 } && ti)
        {
            error_details = ti->files().file_path(ts.error_file);
        }
    }

    TorrentStatus nts;
    nts.addedOn = wxDateTime(ts.added_time);
    nts.allTimeDownload = ts.all_time_download;
    nts.allTimeUpload = ts.all_time_upload;
    nts.availability = ts.distributed_copies;
    nts.completedOn = ts.completed_time > 0 ? wxDateTime(ts.completed_time) : wxDateTime();
    nts.downloadPayloadRate = ts.download_payload_rate;
    nts.error = error;
    nts.errorDetails = error_details;
    nts.eta = eta;
    nts.forced = (!(ts.flags & lt::torrent_flags::paused) && !(ts.flags & lt::torrent_flags::auto_managed));
    nts.infoHash = hash.str();
    nts.lastDownload = ts.last_download.time_since_epoch().count() > 0 ? std::chrono::seconds(lt::total_seconds(lt::clock_type::now() - ts.last_download)) : std::chrono::seconds(-1);
    nts.lastUpload = ts.last_upload.time_since_epoch().count() > 0 ? std::chrono::seconds(lt::total_seconds(lt::clock_type::now() - ts.last_upload)) : std::chrono::seconds(-1);
    nts.name = ts.name.empty() ? nts.infoHash : ts.name;
    nts.paused = (m_th->flags() & lt::torrent_flags::paused) == lt::torrent_flags::paused;
    nts.peersCurrent = ts.num_peers - ts.num_seeds;
    nts.peersTotal = ts.list_peers - ts.list_seeds;
    nts.pieces = ts.pieces;
    nts.progress = ts.progress;
    nts.queuePosition = static_cast<int>(ts.queue_position);
    nts.ratio = ratio;
    nts.savePath = ts.save_path;
    nts.seedsCurrent = ts.num_seeds;
    nts.seedsTotal = ts.list_seeds;
    nts.state = getTorrentStatusState(ts);
    nts.torrentFile = ts.torrent_file;
    nts.totalWanted = ts.total_wanted;
    nts.totalWantedRemaining = ts.total_wanted - ts.total_wanted_done;
    nts.uploadPayloadRate = ts.upload_payload_rate;

    return std::make_unique<TorrentStatus>(nts);
}

lt::torrent_handle& TorrentHandle::WrappedHandle()
{
    return *m_th.get();
}
