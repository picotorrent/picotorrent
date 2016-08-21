#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace libtorrent
{
    class sha1_hash;
    struct torrent_status;
}

namespace ViewModels
{
    class TorrentListViewModel
    {
    public:
        struct Sort
        {
            static void ByDownloadRate(TorrentListViewModel& model, bool ascending);
            static void ByETA(TorrentListViewModel& model, bool ascending);
            static void ByName(TorrentListViewModel& model, bool ascending);
            static void ByPeers(TorrentListViewModel& model, bool ascending);
            static void ByProgress(TorrentListViewModel& model, bool ascending);
            static void ByQueuePosition(TorrentListViewModel& model, bool ascending);
            static void BySeeds(TorrentListViewModel& model, bool ascending);
            static void ByShareRatio(TorrentListViewModel& model, bool ascending);
            static void BySize(TorrentListViewModel& model, bool ascending);
            static void ByStatus(TorrentListViewModel& model, bool ascending);
            static void ByUploadRate(TorrentListViewModel& model, bool ascending);

        private:
            static void Apply(TorrentListViewModel& model, const std::function<bool(const libtorrent::torrent_status& t1, const libtorrent::torrent_status& t2)>& sorter);
        };

        TorrentListViewModel(
            std::vector<libtorrent::sha1_hash>& hashes,
            std::map<libtorrent::sha1_hash, libtorrent::torrent_status>& torrents);
        ~TorrentListViewModel();

        int GetDownloadRate(int index);
        std::chrono::seconds GetETA(int index);
        std::wstring GetName(int index);
        std::pair<int, int> GetPeers(int index);
        float GetProgress(int index);
        int GetQueuePosition(int index);
        std::wstring GetSavePath(int index);
        std::pair<int, int> GetSeeds(int index);
        int64_t GetSize(int index);
        float GetShareRatio(int index);
        int GetUploadRate(int index);
        bool IsPaused(int index);
        void Move(int index, const std::wstring& path);
        void Pause(int index);
        void QueueUp(int index);
        void QueueDown(int index);
        void QueueTop(int index);
        void QueueBottom(int index);
        void Resume(int index, bool force);

    private:
        std::chrono::seconds CalculateETA(const libtorrent::torrent_status& ts);
        float CalculateShareRatio(const libtorrent::torrent_status& ts);

        libtorrent::torrent_status& GetStatus(int index);

        std::vector<libtorrent::sha1_hash>& m_hashes;
        std::map<libtorrent::sha1_hash, libtorrent::torrent_status>& m_torrents;
    };
}
