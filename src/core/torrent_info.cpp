#include <picotorrent/core/torrent_info.hpp>

#include <picotorrent/core/hash.hpp>
#include <picotorrent/core/pal.hpp>
#include <fstream>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_info.hpp>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

namespace lt = libtorrent;
using picotorrent::core::hash;
using picotorrent::core::pal;
using picotorrent::core::torrent_info;

torrent_info::torrent_info(const std::string &buf)
    : info_(new lt::torrent_info(&buf[0], (int)buf.size()))
{
}

torrent_info::torrent_info(const lt::torrent_info &info)
    : info_(new lt::torrent_info(info))
{
}

torrent_info::~torrent_info()
{
}

std::unique_ptr<torrent_info> torrent_info::try_load(const std::string &path, std::string &err)
{
    if (!pal::file_exists(path))
    {
        return nullptr;
    }

    std::ifstream file_stream(path, std::ios::binary);
    if (!file_stream) { return nullptr; }

    std::stringstream ss;
    ss << file_stream.rdbuf();
    std::string buf = ss.str();

    lt::error_code ec;
    lt::torrent_info ti(&buf[0], (int)buf.size(), ec);

    if (ec)
    {
        err = ec.message();
        return nullptr;
    }

    return std::unique_ptr<core::torrent_info>(new torrent_info(ti));
}

std::string torrent_info::file_path(int index) const
{
    return info_->files().file_path(index);
}

int64_t torrent_info::file_size(int index) const
{
    return info_->files().file_size(index);
}

std::shared_ptr<hash> torrent_info::info_hash() const
{
    return std::make_shared<hash>(info_->info_hash());
}

std::string torrent_info::name()
{
    return info_->name();
}

int torrent_info::num_files() const
{
    return info_->num_files();
}

int64_t torrent_info::total_size()
{
    return info_->total_size();
}
