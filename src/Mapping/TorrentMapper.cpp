#include "TorrentMapper.hpp"

#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <picotorrent/api.hpp>

namespace lt = libtorrent;
using Mapping::TorrentMapper;

Torrent TorrentMapper::Map(libtorrent::torrent_status const& status)
{
    bool isPaused = status.paused && !status.auto_managed;
    float ratio = 0;
    int eta = -1;
    int pieceLength = -1;
    int piecesCount = -1;
    Torrent::State state = Torrent::State::Unknown;

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
            state = Torrent::State::Error;
        }
        else
        {
            if (isSeeding)
            {
                state = Torrent::State::UploadingPaused;
            }
            else
            {
                state = Torrent::State::DownloadingPaused;
            }
        }
    }
    else
    {
        if (isQueued && !isChecking)
        {
            if (isSeeding)
            {
                state = Torrent::State::UploadingQueued;
            }
            else
            {
                state = Torrent::State::DownloadingQueued;
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
                    state = Torrent::State::UploadingForced;
                }
                else if (isPaused)
                {
                    state = Torrent::State::Complete;
                }
                else
                {
                    if (status.upload_payload_rate > 0)
                    {
                        state = Torrent::State::Uploading;
                    }
                    else
                    {
                        state = Torrent::State::UploadingStalled;
                    }
                }
                break;
            }

            case lt::torrent_status::state_t::checking_resume_data:
            {
                state = Torrent::State::CheckingResumeData;
                break;
            }

            case lt::torrent_status::state_t::checking_files:
            {
                state = Torrent::State::DownloadingChecking;
                break;
            }

            case lt::torrent_status::state_t::downloading_metadata:
            {
                state = Torrent::State::DownloadingMetadata;
                break;
            }

            case lt::torrent_status::state_t::downloading:
            {
                if (isForced)
                {
                    state = Torrent::State::DownloadingForced;
                }
                else
                {
                    if (status.download_payload_rate > 0)
                    {
                        state = Torrent::State::Downloading;
                    }
                    else
                    {
                        state = Torrent::State::DownloadingStalled;
                    }
                }
                break;
            }
            }
        }
    }

	std::stringstream ss;
	ss << status.info_hash;

    int64_t totalSize = -1;

    if (status.handle.torrent_file())
    {
        totalSize = status.handle.torrent_file()->total_size();
    }

    return Torrent
    {
        ss.str(),
        status.name.empty() ? ss.str() : status.name,
        status.queue_position,
        totalSize,
        status.total_wanted,
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
        status.save_path,
        status.all_time_download,
        status.all_time_upload,
        status.num_pieces,
        pieceLength,
        piecesCount,
        status.errc ? status.errc.message() : ""
    };
}
