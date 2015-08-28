#include "addtorrentcontroller.h"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session_handle.hpp>

AddTorrentController::AddTorrentController(const libtorrent::session_handle& session,
    std::vector<libtorrent::add_torrent_params> params)
    : session_(session),
    params_(params)
{
}

void AddTorrentController::Add()
{
}

int AddTorrentController::GetCount()
{
    return params_.size();
}

std::string AddTorrentController::GetName(int index)
{
    return params_[index].ti->name();
}

int64_t AddTorrentController::GetSize(int index)
{
    return params_[index].ti->total_size();
}

std::string AddTorrentController::GetComment(int index)
{
    return params_[index].ti->comment();
}

std::string AddTorrentController::GetCreationDate(int index)
{
    boost::optional<time_t> time = params_[index].ti->creation_date();

    if (!time)
    {
        return "-";
    }

    char fd[100];
    std::time_t tm(time.value());
    std::strftime(fd, sizeof(fd), "%c", std::localtime(&tm));

    return fd;
}

std::string AddTorrentController::GetCreator(int index)
{
    return params_[index].ti->creator();
}

std::string AddTorrentController::GetSavePath(int index)
{
    return params_[index].save_path;
}

int AddTorrentController::GetFileCount(int index)
{
    return params_[index].ti->num_files();
}

int AddTorrentController::GetFilePriority(int index, int fileIndex)
{
    libtorrent::add_torrent_params& p = params_[index];

    if (p.file_priorities.size() > 0)
    {
        return p.file_priorities[fileIndex];
    }

    return 1; // TODO put in enum
}

std::string AddTorrentController::GetFileName(int index, int fileIndex)
{
    return params_[index].ti->files().file_name(fileIndex);
}

int64_t AddTorrentController::GetFileSize(int index, int fileIndex)
{
    return params_[index].ti->files().file_size(fileIndex);
}

void AddTorrentController::SetFileName(int index, int fileIndex, const std::string& name)
{
    params_[index].ti->rename_file(fileIndex, name);
}

void AddTorrentController::SetFilePriority(int index, int fileIndex, int prio)
{
    libtorrent::add_torrent_params& p = params_[index];

    if (p.file_priorities.size() == 0)
    {
        // Resize the file prio vector with default value of 1.
        p.file_priorities.resize(fileIndex + 1, 1);
    }

    p.file_priorities[fileIndex] = prio;
}

void AddTorrentController::SetSavePath(int index, std::string savePath)
{
    params_[index].save_path = savePath;
}
