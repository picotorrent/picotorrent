#pragma once

#include <chrono>
#include <string>

#include <libtorrent/sha1_hash.hpp>

namespace libtorrent { struct torrent_status; }

namespace Models
{
    struct Torrent
    {
        enum State
        {
            Unknown = -1,
            CheckingResumeData,
            Downloading,
            DownloadingChecking,
            DownloadingForced,
            DownloadingMetadata,
            DownloadingPaused,
            DownloadingQueued,
            DownloadingStalled,
            Error,
            Uploading,
            UploadingChecking,
            UploadingForced,
            UploadingPaused,
            UploadingQueued,
            UploadingStalled
        };

        static Torrent Map(const libtorrent::sha1_hash& hash);
        static Torrent Map(const libtorrent::torrent_status& status);

        bool operator==(const Torrent& other)
        {
            return infoHash == other.infoHash;
        }

        bool operator!=(const Torrent& other)
        {
            return !(*this == other);
        }

        libtorrent::sha1_hash infoHash;
        std::wstring name;
        int queuePosition;
        int64_t size;
        State state;
        float progress;
        std::chrono::seconds eta;
        int downloadRate;
        int uploadRate;
        int seedsConnected;
        int seedsTotal;
        int peersConnected;
        int peersTotal;
        float shareRatio;
        bool isPaused;
        std::wstring savePath;
        int64_t downloadedBytes;
        int64_t uploadedBytes;
        int piecesHave;
        int pieceLength;
        int piecesCount;
        std::wstring errorMessage;
    };
}
