#pragma once

namespace pt::BitTorrent
{
    class TorrentHandle;
}

namespace pt::UI::Filters
{
    class TorrentFilter
    {
    public:
        virtual ~TorrentFilter() {}

        virtual bool Includes(BitTorrent::TorrentHandle const& torrent) = 0;
    };
}
