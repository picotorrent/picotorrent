#include "torrenthandle.hpp"

#include <QDir>

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include "session.hpp"
#include "torrentstatus.hpp"

namespace lt = libtorrent;
using pt::TorrentHandle;

pt::TorrentStatus::State getTorrentStatusState(lt::torrent_status const& ts)
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
            return pt::TorrentStatus::State::Error;
        }

        if (seeding)
        {
            return pt::TorrentStatus::State::UploadingPaused;
        }

        return pt::TorrentStatus::State::DownloadingPaused;
    }
    else
    {
        if (queued && !checking)
        {
            return seeding
                ? pt::TorrentStatus::State::UploadingQueued
                : pt::TorrentStatus::State::DownloadingQueued;
        }
        else
        {
            switch (ts.state)
            {
            case lt::torrent_status::state_t::finished:
            case lt::torrent_status::state_t::seeding:
                return paused
                    ? pt::TorrentStatus::State::UploadingPaused
                    : pt::TorrentStatus::State::Uploading;

            case lt::torrent_status::state_t::checking_resume_data:
                return pt::TorrentStatus::State::CheckingResumeData;

            case lt::torrent_status::state_t::checking_files:
                return pt::TorrentStatus::State::DownloadingChecking;

            case lt::torrent_status::state_t::downloading_metadata:
                return pt::TorrentStatus::State::DownloadingMetadata;

            case lt::torrent_status::state_t::downloading:
                return pt::TorrentStatus::State::Downloading;
            }
        }
    }

    return pt::TorrentStatus::State::Unknown;
}

TorrentHandle::TorrentHandle(pt::Session* session, lt::torrent_handle const& th)
    : m_session(session)
{
    m_th = std::make_unique<lt::torrent_handle>(th);
    m_ts = std::make_unique<lt::torrent_status>(th.status());
}

TorrentHandle::~TorrentHandle()
{
}

void TorrentHandle::addTracker(lt::announce_entry const& entry)
{
    m_th->add_tracker(entry);
}

void TorrentHandle::fileProgress(std::vector<std::int64_t>& progress, int flags) const
{
    m_th->file_progress(progress, flags);
}

void TorrentHandle::forceReannounce()
{
    m_th->force_reannounce();
}

void TorrentHandle::forceReannounce(int seconds, int trackerIndex)
{
    m_th->force_reannounce(seconds, trackerIndex);
}

void TorrentHandle::forceRecheck()
{
    /*th.force_recheck();

    lt::torrent_status ts = th.status();

    bool paused = ((ts.flags & lt::torrent_flags::paused)
        && !(ts.flags & lt::torrent_flags::auto_managed));

    if (paused)
    {
        th.resume();
        m_state->pauseAfterChecking.insert(hash);
    }*/

    m_th->force_recheck();
}

std::vector<lt::download_priority_t> TorrentHandle::getFilePriorities() const
{
    return m_th->get_file_priorities();
}

void TorrentHandle::getPeerInfo(std::vector<lt::peer_info>& peers) const
{
    m_th->get_peer_info(peers);
}

lt::info_hash_t TorrentHandle::infoHash()
{
    return m_th->info_hash();
}

bool TorrentHandle::isValid()
{
    return m_th->is_valid();
}

void TorrentHandle::moveStorage(QString const& newPath)
{
    QString native = QDir::toNativeSeparators(newPath);
    m_th->move_storage(native.toStdString());
}

void TorrentHandle::pause()
{
    m_th->unset_flags(lt::torrent_flags::auto_managed);
    m_th->pause(lt::torrent_handle::graceful_pause);
}

void TorrentHandle::queueUp()
{
    m_th->queue_position_up();
}

void TorrentHandle::queueDown()
{
    m_th->queue_position_down();
}

void TorrentHandle::queueTop()
{
    m_th->queue_position_top();
}

void TorrentHandle::queueBottom()
{
    m_th->queue_position_bottom();
}

void TorrentHandle::replaceTrackers(std::vector<lt::announce_entry> const& trackers)
{
    m_th->replace_trackers(trackers);
}

void TorrentHandle::remove()
{
    m_session->removeTorrent(this);
}

void TorrentHandle::removeFiles()
{
    m_session->removeTorrent(this, lt::session_handle::delete_files);
}

void TorrentHandle::resume()
{
    m_th->set_flags(lt::torrent_flags::auto_managed);
    m_th->clear_error();
    m_th->resume();
}

void TorrentHandle::resumeForce()
{
    m_th->unset_flags(lt::torrent_flags::auto_managed);
    m_th->clear_error();
    m_th->resume();
}

void TorrentHandle::setFilePriorities(std::vector<lt::download_priority_t> priorities)
{
    m_th->prioritize_files(priorities);
}

void TorrentHandle::setFilePriority(lt::file_index_t index, lt::download_priority_t priority)
{
    m_th->file_priority(index, priority);
}

pt::TorrentStatus TorrentHandle::status()
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
        ts.infoHash = QString::fromStdString(hash.str());
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
    ts.addedOn              = QDateTime::fromSecsSinceEpoch(m_ts->added_time);
    ts.availability         = m_ts->distributed_copies;
    ts.completedOn          = m_ts->completed_time > 0 ? QDateTime::fromSecsSinceEpoch(m_ts->completed_time) : QDateTime();
    ts.downloadPayloadRate  = m_ts->download_payload_rate;
    ts.error                = m_ts->errc ? QString::fromStdString(m_ts->errc.message()) : "";
    ts.eta                  = eta;
    ts.forced               = (!(m_ts->flags & lt::torrent_flags::paused) && !(m_ts->flags & lt::torrent_flags::auto_managed));
    ts.infoHash             = QString::fromStdString(hash.str());
    ts.name                 = QString::fromStdString(m_ts->name);
    ts.paused               = (m_th->flags() & lt::torrent_flags::paused) == lt::torrent_flags::paused;
    ts.peersCurrent         = m_ts->num_peers - m_ts->num_seeds;
    ts.peersTotal           = m_ts->list_peers - m_ts->list_seeds;
    ts.pieces               = m_ts->pieces;
    ts.progress             = m_ts->progress;
    ts.queuePosition        = static_cast<int>(m_ts->queue_position);
    ts.ratio                = ratio;
    ts.savePath             = QString::fromStdString(m_ts->save_path);
    ts.seedsCurrent         = m_ts->num_seeds;
    ts.seedsTotal           = m_ts->list_seeds;
    ts.state                = getTorrentStatusState(*m_ts.get());
    ts.torrentFile          = m_ts->torrent_file;
    ts.totalWanted          = m_ts->total_wanted;
    ts.totalWantedRemaining = m_ts->total_wanted - m_ts->total_wanted_done;
    ts.uploadPayloadRate    = m_ts->upload_payload_rate;
    return ts;
}

std::vector<lt::announce_entry> TorrentHandle::trackers() const
{
    return m_th->trackers();
}

void TorrentHandle::updateStatus(lt::torrent_status const& ts)
{
    m_ts = std::make_unique<lt::torrent_status>(ts);
}

lt::torrent_handle& TorrentHandle::wrappedHandle()
{
    return *m_th.get();
}
