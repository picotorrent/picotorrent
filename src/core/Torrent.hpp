#pragma once

#include <boost/shared_ptr.hpp>

namespace libtorrent { class torrent_info; }

namespace Core
{
    class Torrent
    {
    public:
        static void Save(const boost::shared_ptr<const libtorrent::torrent_info>& ti, std::error_code& ec);
    };
}
