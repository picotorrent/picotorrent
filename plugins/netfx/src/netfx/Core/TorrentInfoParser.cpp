#include "TorrentInfoParser.h"

#include "TorrentInfo.h"

#include <msclr/marshal_cppstd.h>
#include <picotorrent/core/torrent_info.hpp>

using picotorrent::core::torrent_info;

using PicoTorrent::Core::ITorrentInfo;
using PicoTorrent::Core::TorrentInfo;
using PicoTorrent::Core::TorrentInfoParser;

ITorrentInfo^ TorrentInfoParser::Parse(System::String^ fileName)
{
    std::string file = msclr::interop::marshal_as<std::string>(fileName);
    std::string err;

    auto ti = torrent_info::try_load(file, err);

    if (ti == nullptr || !err.empty())
    {
        return nullptr;
    }

    return nullptr;
}
