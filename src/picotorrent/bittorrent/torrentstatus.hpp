#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <cstdint>
#include <memory>
#include <string>

#include <libtorrent/bitfield.hpp>
#include <libtorrent/torrent_info.hpp>
#include <wx/datetime.h>

namespace pt
{
namespace BitTorrent
{
    struct TorrentStatus
    {
        enum State
        {
            Unknown,
            Error,
            CheckingFiles,
            CheckingResumeData,
            Downloading,
            DownloadingChecking,
            DownloadingMetadata,
            DownloadingPaused,
            DownloadingQueued,
            Uploading,
            UploadingPaused,
            UploadingQueued
        };

        wxDateTime                                    addedOn;
        float                                         availability;
        wxDateTime                                    completedOn;
        int                                           downloadPayloadRate;
        bool                                          forced;
        std::string                                   error;
        std::chrono::seconds                          eta;
        std::string                                   infoHash;
        std::string                                   name;
        bool                                          paused;
        int                                           peersCurrent;
        int                                           peersTotal;
        libtorrent::bitfield                          pieces;
        float                                         progress;
        int                                           queuePosition;
        float                                         ratio;
        std::string                                   savePath;
        int                                           seedsCurrent;
        int                                           seedsTotal;
        State                                         state;
        std::weak_ptr<const libtorrent::torrent_info> torrentFile;
        std::int64_t                                  totalWanted;
        std::int64_t                                  totalWantedRemaining;
        int                                           uploadPayloadRate;
    };
}
}
