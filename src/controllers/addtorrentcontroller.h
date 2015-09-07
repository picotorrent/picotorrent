#pragma once

#include <string>
#include <vector>

namespace libtorrent
{
    struct add_torrent_params;
    struct session_handle;
}

namespace pico
{
    class AddTorrentController
    {
    public:
        AddTorrentController(libtorrent::session_handle& session,
            std::vector<libtorrent::add_torrent_params> params);

        void AddTorrents();

        uint64_t GetCount();
        std::wstring GetName(uint64_t index);
        std::wstring GetSize(uint64_t index);
        std::wstring GetComment(uint64_t index);
        std::wstring GetCreationDate(uint64_t index);
        std::wstring GetCreator(uint64_t index);
        std::wstring GetSavePath(uint64_t index);

        int GetFileCount(uint64_t index);
        std::wstring GetFileName(uint64_t index, int fileIndex);
        std::wstring GetFileSize(uint64_t index, int fileIndex);
        uint8_t GetFilePriority(uint64_t index, int fileIndex);

        void SetFileName(int64_t index, int fileIndex, const std::wstring& name);
        void SetFilePriority(int64_t index, int fileIndex, uint8_t priority);
        void SetSavePath(uint64_t index, const std::wstring& path);

    private:
        libtorrent::session_handle& session_;
        std::vector<libtorrent::add_torrent_params> params_;
    };
}
