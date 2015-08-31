#ifndef _PT_CONTROLLERS_ADDTORRENTCONTROLLER_H
#define _PT_CONTROLLERS_ADDTORRENTCONTROLLER_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace libtorrent
{
    struct add_torrent_params;
    struct session_handle;
}

class AddTorrentController
{
public:
    AddTorrentController(const libtorrent::session_handle& session,
        std::vector<libtorrent::add_torrent_params> params);

    void Add();

    int GetCount();

    std::string GetName(int index);

    int64_t GetSize(int index);

    std::string GetComment(int index);

    std::string GetCreationDate(int index);

    std::string GetCreator(int index);

    std::string GetSavePath(int index);

    int GetFileCount(int index);

    int GetFilePriority(int index, int fileIndex);

    std::string GetFileName(int index, int fileIndex);

    int64_t GetFileSize(int index, int fileIndex);

    void SetFileName(int index, int fileIndex, const std::string& name);

    void SetFilePriority(int index, int fileIndex, int prio);

    void SetSavePath(int index, std::string savePath);
    
private:
    const libtorrent::session_handle& session_;
    std::vector<libtorrent::add_torrent_params> params_;
};

#endif
