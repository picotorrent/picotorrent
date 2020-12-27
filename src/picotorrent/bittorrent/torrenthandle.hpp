#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <vector>

#include <libtorrent/download_priority.hpp>
#include <libtorrent/fwd.hpp>
#include <libtorrent/sha1_hash.hpp>

namespace pt
{
namespace BitTorrent
{
    class Session;
    struct TorrentStatus;

    class TorrentHandle : public wxEvtHandler
    {
        friend class Session;

    public:
        virtual ~TorrentHandle();

        void AddTracker(libtorrent::announce_entry const& entry);
        void FileProgress(std::vector<std::int64_t>& progress, int flags) const;
        std::vector<libtorrent::download_priority_t> GetFilePriorities() const;
        void GetPeerInfo(std::vector<libtorrent::peer_info>& peers) const;
        libtorrent::info_hash_t InfoHash();
        bool IsSequentialDownload();
        bool IsValid();
        void ReplaceTrackers(std::vector<libtorrent::announce_entry> const& trackers);
        void ScrapeTracker(int trackerIndex);
        TorrentStatus Status() const;
        std::vector<libtorrent::announce_entry> Trackers() const;

        void ForceReannounce();
        void ForceReannounce(int seconds, int trackerIndex);
        void ForceRecheck();
        void MoveStorage(std::string const& newPath);
        void Pause();
        void QueueUp();
        void QueueDown();
        void QueueTop();
        void QueueBottom();
        void Remove();
        void RemoveFiles();
        void Resume();
        void ResumeForce();
        void SetFilePriorities(std::vector<libtorrent::download_priority_t> priorities);
        void SetFilePriority(libtorrent::file_index_t index, libtorrent::download_priority_t priority);
        void SetSequentialDownload(bool seq);

        // Labels
        int Label();
        void ClearLabel();
        void SetLabel(int id, std::string const& name);

    private:
        TorrentHandle(Session* session, libtorrent::torrent_handle const& th);

        void BuildStatus(libtorrent::torrent_status const& ts);
        void SetLabelMuted(int labelId);
        std::unique_ptr<TorrentStatus> Update(libtorrent::torrent_status const& ts);
        libtorrent::torrent_handle& WrappedHandle();

        Session* m_session;
        std::unique_ptr<libtorrent::torrent_handle> m_th;
        std::unique_ptr<TorrentStatus> m_status;
        int m_labelId;
        std::string m_labelName;
    };
}
}
