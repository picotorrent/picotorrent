#include <picotorrent/core/torrent_file.hpp>

#include <libtorrent/torrent_info.hpp>

namespace lt = libtorrent;
using picotorrent::core::torrent_file;

torrent_file::torrent_file(const std::vector<char> &buf)
    : info_(std::make_unique<lt::torrent_info>(&buf[0], buf.size()))
{
}

torrent_file::torrent_file(const lt::torrent_info &info)
    : info_(std::make_unique<lt::torrent_info>(info))
{
}

torrent_file::~torrent_file()
{
}
