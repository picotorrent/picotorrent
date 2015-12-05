#include <picotorrent/core/torrent_info.hpp>

#include <libtorrent/torrent_info.hpp>
#include <picotorrent/filesystem/file.hpp>
#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/logging/log.hpp>

namespace lt = libtorrent;
namespace fs = picotorrent::filesystem;
using picotorrent::core::torrent_info;

torrent_info::torrent_info(const std::vector<char> &buf)
    : info_(std::make_unique<lt::torrent_info>(&buf[0], buf.size()))
{
}

torrent_info::torrent_info(const lt::torrent_info &info)
    : info_(std::make_unique<lt::torrent_info>(info))
{
}

torrent_info::~torrent_info()
{
}

std::shared_ptr<torrent_info> torrent_info::try_load(const fs::path &path)
{
    if (!path.exists())
    {
        return nullptr;
    }

    fs::file f(path);
    std::vector<char> buf;

    try
    {
        f.read_all(buf);
    }
    catch (const std::exception &e)
    {
        LOG(error) << "Error when reading file: " << e.what();
        return nullptr;
    }

    return std::make_shared<torrent_info>(buf);
}

std::string torrent_info::file_name(int index)
{
    return info_->files().file_name(index);
}

int64_t torrent_info::file_size(int index)
{
    return info_->files().file_size(index);
}

std::string torrent_info::name()
{
    return info_->name();
}

int torrent_info::num_files()
{
    return info_->num_files();
}

int64_t torrent_info::total_size()
{
    return info_->total_size();
}
