#include "addtorrentcontroller.h"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session_handle.hpp>
#include <libtorrent/torrent_info.hpp>

#include "../util.h"

namespace lt = libtorrent;
using namespace pico;

AddTorrentController::AddTorrentController(lt::session_handle& session,
    std::vector<lt::add_torrent_params> params)
    : session_(session),
    params_(params)
{
}

void AddTorrentController::AddTorrents()
{
    for (lt::add_torrent_params& p : params_)
    {
        session_.async_add_torrent(p);
    }
}

uint64_t AddTorrentController::GetCount()
{
    return params_.size();
}

std::wstring AddTorrentController::GetName(uint64_t index)
{
    std::string name = params_[index].ti->name();
    return Util::ToWideString(name);
}

std::wstring AddTorrentController::GetSize(uint64_t index)
{
    int64_t size = params_[index].ti->total_size();

    if (size < 0)
    {
        return L"-";
    }

    return Util::ToFileSize(size);
}

std::wstring AddTorrentController::GetComment(uint64_t index)
{
    std::string comment = params_[index].ti->comment();

    if (comment.empty())
    {
        return L"-";
    }

    return Util::ToWideString(comment);
}

std::wstring AddTorrentController::GetCreationDate(uint64_t index)
{
    boost::optional<time_t> t = params_[index].ti->creation_date();

    if (!t.is_initialized())
    {
        return L"-";
    }

    tm* time = localtime(&t.get());
    
    char buf[100];
    strftime(buf, _ARRAYSIZE(buf), "%c", time);

    return Util::ToWideString(buf);
}

std::wstring AddTorrentController::GetCreator(uint64_t index)
{
    std::string creator = params_[index].ti->creator();

    if (creator.empty())
    {
        return L"-";
    }

    return Util::ToWideString(creator);
}

std::wstring AddTorrentController::GetSavePath(uint64_t index)
{
    std::string path = params_[index].save_path;
    return Util::ToWideString(path);
}

uint64_t AddTorrentController::GetFileCount(uint64_t index)
{
    return params_[index].ti->num_files();
}

std::wstring AddTorrentController::GetFileName(uint64_t index, int fileIndex)
{
    std::string name = params_[index].ti->files().file_name(fileIndex);
    return Util::ToWideString(name);
}

std::wstring AddTorrentController::GetFileSize(uint64_t index, int fileIndex)
{
    int64_t size = params_[index].ti->files().file_size(fileIndex);
    return Util::ToFileSize(size);
}

int AddTorrentController::GetFilePriority(uint64_t index, int fileIndex)
{
    lt::add_torrent_params& p = params_[index];

    if (p.file_priorities.size() > fileIndex)
    {
        return p.file_priorities[fileIndex];
    }

    return 1; // TODO(enum)
}

void AddTorrentController::SetFileName(int64_t index, int fileIndex, const std::wstring& name)
{
    params_[index].ti->rename_file(fileIndex, Util::ToString(name));
}

void AddTorrentController::SetFilePriority(int64_t index, int fileIndex, int priority)
{
    lt::add_torrent_params& p = params_[index];

    if (fileIndex >= p.file_priorities.size())
    {
        p.file_priorities.resize(fileIndex + 1, 1);
    }

    p.file_priorities[fileIndex] = priority;
}

void AddTorrentController::SetSavePath(uint64_t index, const std::wstring& path)
{
    params_[index].save_path = Util::ToString(path);
}
