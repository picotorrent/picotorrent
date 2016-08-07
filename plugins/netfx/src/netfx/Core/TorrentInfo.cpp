#include "TorrentInfo.h"

#include <picotorrent/core/hash.hpp>
#include <picotorrent/core/torrent_info.hpp>

#include <msclr/marshal_cppstd.h>

using PicoTorrent::Core::TorrentInfo;

TorrentInfo::TorrentInfo(picotorrent::core::torrent_info* ti)
    : ti_(ti)
{
}

TorrentInfo::~TorrentInfo()
{
    delete ti_;
}

System::String^ TorrentInfo::InfoHash::get()
{
    return msclr::interop::marshal_as<System::String^>(ti_->info_hash()->to_string());
}
