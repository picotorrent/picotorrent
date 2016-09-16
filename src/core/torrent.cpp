#include "Torrent.hpp"

#include <sstream>

#include <libtorrent/create_torrent.hpp>
#include <libtorrent/torrent_info.hpp>

#include "../Environment.hpp"
#include "../IO/Directory.hpp"
#include "../IO/File.hpp"
#include "../IO/Path.hpp"
#include "../StringUtils.hpp"

namespace lt = libtorrent;
using Core::Torrent;

void Torrent::Save(const std::shared_ptr<const lt::torrent_info>& ti, std::error_code& ec)
{
    lt::create_torrent ct(*ti);
    lt::entry e = ct.generate();

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), e);

    std::wstring torrents_dir = IO::Path::Combine(Environment::GetDataPath(), TEXT("Torrents"));
    if (!IO::Directory::Exists(torrents_dir)) { IO::Directory::Create(torrents_dir); }

    std::stringstream hex;
    hex << ti->info_hash();
    std::string file_name = hex.str() + ".torrent";

    std::wstring torrent_file = IO::Path::Combine(torrents_dir, TWS(file_name));
    IO::File::WriteAllBytes(torrent_file, buf, ec);
}
