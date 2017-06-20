#include "Torrent.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include <libtorrent/create_torrent.hpp>
#include <libtorrent/torrent_info.hpp>

#include "../Environment.hpp"
#include "../StringUtils.hpp"

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using Core::Torrent;

void Torrent::Save(const std::shared_ptr<const lt::torrent_info>& ti, std::error_code& ec)
{
    lt::create_torrent ct(*ti);
    lt::entry e = ct.generate();

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), e);

    fs::path torrents_dir = fs::path(Environment::GetDataPath()) / "Torrents";
    if (!fs::exists(torrents_dir)) { fs::create_directories(torrents_dir); }

    std::stringstream hex;
    hex << ti->info_hash();
    std::string file_name = hex.str() + ".torrent";

    fs::path torrent_file = torrents_dir / file_name;

    std::ofstream out(torrent_file, std::ios::binary | std::ios::out);
    std::copy(
        buf.begin(),
        buf.end(),
        std::ostreambuf_iterator<char>(out));
}
