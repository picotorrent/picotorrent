#pragma once

namespace libtorrent
{
    struct torrent_status;
}

struct Torrent;

namespace Mapping
{
    struct TorrentMapper
    {
        static Torrent Map(libtorrent::torrent_status const& ts);
    };
}
