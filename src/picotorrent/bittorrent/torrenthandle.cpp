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

    if (paused)
    {
        if (ts.errc)
        {
            return TorrentStatus::State::Error;
        }

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
    m_th->file_progress(progress, flags);
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
    return m_th->info_hash();
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

void TorrentHandle::SetFilePriorities(std::vector<lt::download_priority_t> priorities)
{
    m_th->prioritize_files(priorities);
}

void TorrentHandle::SetFilePriority(lt::file_index_t index, lt::download_priority_t priority)
{
    m_th->file_priority(index, priority);
}

TorrentStatus TorrentHandle::Status()
{
    std::stringstream hash;

    if (m_ts->info_hash.has_v2())
    {
        hash << m_ts->info_hash.v2;
    }
    else
    {
        hash << m_ts->info_hash.v1;
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

    TorrentStatus ts;
    ts.addedOn              = wxDateTime(m_ts->added_time);
    ts.availability         = m_ts->distributed_copies;
    ts.completedOn          = m_ts->completed_time > 0 ? wxDateTime(m_ts->completed_time) : wxDateTime();
    ts.downloadPayloadRate  = m_ts->download_payload_rate;
    ts.error                = m_ts->errc ? m_ts->errc.message() : "";
    ts.eta                  = eta;
    ts.forced               = (!(m_ts->flags & lt::torrent_flags::paused) && !(m_ts->flags & lt::torrent_flags::auto_managed));
    ts.infoHash             = hash.str();
    ts.name                 = m_ts->name;
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
