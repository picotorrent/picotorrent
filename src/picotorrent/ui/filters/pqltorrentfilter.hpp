#pragma once

#include <functional>
#include <memory>
#include <string>

#include "torrentfilter.hpp"

namespace pt::BitTorrent { struct TorrentStatus; }

namespace pt::UI::Filters
{
    class PqlTorrentFilter : public TorrentFilter
    {
    public:
        static std::unique_ptr<TorrentFilter> Create(std::string const& input, std::string* error);

        ~PqlTorrentFilter();
        bool Includes(BitTorrent::TorrentHandle const& torrent);

    private:
        PqlTorrentFilter(std::function<bool(BitTorrent::TorrentStatus const&)> const& filter);

        std::function<bool(BitTorrent::TorrentStatus const&)> m_filter;
    };
}
