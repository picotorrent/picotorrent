#include "Torrent.hpp"

#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

#include "../StringUtils.hpp"

namespace lt = libtorrent;
using Models::Torrent;

Torrent Torrent::Map(const lt::sha1_hash& hash)
{
    return Torrent{ hash };
}

Torrent Torrent::Map(const lt::torrent_status& status)
{
    bool isPaused = status.paused && !status.auto_managed;
    float ratio = 0;
    int eta = -1;
    int pieceLength = -1;
    int piecesCount = -1;
    State state = State::Unknown;

    if (status.all_time_download > 0)
    {
        ratio = (float)status.all_time_upload / (float)status.all_time_download;
    }

    if (!isPaused)
    {
        int64_t remaining_bytes = status.total_wanted - status.total_wanted_done;
        if (remaining_bytes > 0 && status.download_payload_rate > 0)
        {
            eta = (int)(remaining_bytes / status.download_payload_rate);
        }
    }

    if (status.handle.torrent_file())
    {
        pieceLength = status.handle.torrent_file()->piece_length();
    }

    if (status.handle.torrent_file())
    {
        piecesCount = status.handle.torrent_file()->num_pieces();
    }

    /*
    State
    */
    bool hasError = status.paused && status.errc;
    bool isSeeding = (status.state == lt::torrent_status::state_t::finished
        || status.state == lt::torrent_status::state_t::seeding);
    bool isQueued = (status.paused && status.auto_managed);
    bool isChecking = (status.state == lt::torrent_status::state_t::checking_files
        || status.state == lt::torrent_status::state_t::checking_resume_data);
    bool isForced = (!status.paused && !status.auto_managed);

    if (isPaused)
    {
        if (hasError)
        {
            state = State::Error;
        }
        else
        {
            if (isSeeding)
            {
                state = State::UploadingPaused;
            }
            else
            {
                state = State::DownloadingPaused;
            }
        }
    }
    else
    {
        if (isQueued && !isChecking)
        {
            if (isSeeding)
            {
                state = State::UploadingQueued;
            }
            else
            {
                state = State::DownloadingQueued;
            }
        }
        else
        {
            switch (status.state)
            {
            case lt::torrent_status::state_t::finished:
            case lt::torrent_status::state_t::seeding:
            {
                if (isForced)
                {
                    state = State::UploadingForced;
                }
                else
                {
                    if (status.upload_payload_rate > 0)
                    {
                        state = State::Uploading;
                    }
                    else
                    {
                        state = State::UploadingStalled;
                    }
                }
                break;
            }

            case lt::torrent_status::state_t::checking_resume_data:
            {
                state = State::CheckingResumeData;
                break;
            }

            case lt::torrent_status::state_t::checking_files:
            {
                state = State::DownloadingChecking;
                break;
            }

            case lt::torrent_status::state_t::downloading_metadata:
            {
                state = State::DownloadingMetadata;
                break;
            }

            case lt::torrent_status::state_t::downloading:
            {
                if (isForced)
                {
                    state = State::DownloadingForced;
                }
                else
                {
                    if (status.download_payload_rate > 0)
                    {
                        state = State::Downloading;
                    }
                    else
                    {
                        state = State::DownloadingStalled;
                    }
                }
                break;
            }
            }
        }
    }

    return Torrent
    {
        status.info_hash,
        TWS(status.name),
        status.queue_position,
        status.handle.torrent_file()->total_size(),
        state,
        status.progress,
        std::chrono::seconds(eta),
        status.download_payload_rate,
        status.upload_payload_rate,
        status.num_seeds,
        status.list_seeds,
        status.num_peers - status.num_seeds,
        status.list_peers - status.list_seeds,
        ratio,
        isPaused,
        TWS(status.save_path),
        status.all_time_download,
        status.all_time_upload,
        status.num_pieces,
        pieceLength,
        piecesCount,
        TWS(status.errc ? status.errc.message() : "")
    };
}
