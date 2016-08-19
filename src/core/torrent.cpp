#include "Torrent.hpp"

#include <libtorrent/create_torrent.hpp>
#include <libtorrent/torrent_info.hpp>

#include "../Environment.hpp"
#include "../IO/Directory.hpp"
#include "../IO/File.hpp"
#include "../IO/Path.hpp"
#include "../StringUtils.hpp"

namespace lt = libtorrent;
using Core::Torrent;

void Torrent::Save(const boost::shared_ptr<const lt::torrent_info>& ti, std::error_code& ec)
{
    lt::create_torrent ct(*ti);
    lt::entry e = ct.generate();

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), e);

    std::wstring torrents_dir = IO::Path::Combine(Environment::GetDataPath(), TEXT("Torrents"));
    if (!IO::Directory::Exists(torrents_dir)) { IO::Directory::Create(torrents_dir); }

    std::string hash = lt::to_hex(ti->info_hash().to_string());
    std::string file_name = hash + ".torrent";

    std::wstring torrent_file = IO::Path::Combine(torrents_dir, TWS(file_name));
    IO::File::WriteAllBytes(torrent_file, buf, ec);
}
