#pragma once

#include <string>

namespace libtorrent
{
    struct torrent_status;
}

namespace pico
{
    class TorrentItem
    {
    public:
        TorrentItem(libtorrent::torrent_status const& status);

        int const GetDownloadRate();
        int const GetUploadRate();
        std::wstring const& GetName();
        float const GetProgress();
        int const GetQueuePosition();
        int64_t const GetSize();
        int const GetStatus();
        void SetDownloadRate(int rate);
        void SetUploadRate(int rate);
        void SetName(std::wstring const& name);
        void SetProgress(float progress);
        void SetQueuePosition(int queuePosition);
        void SetSize(size_t size);
        void SetStatus(int status);

    private:
        int downloadRate_;
        int uploadRate_;
        std::wstring name_;
        float progress_;
        int queuePosition_;
        size_t size_;
        int status_;
    };
}
