#pragma once

#include <memory>
#include <system_error>

namespace libtorrent { class torrent_info; }

namespace Core
{
    class Torrent
    {
    public:
        static void Save(const std::shared_ptr<const libtorrent::torrent_info>& ti, std::error_code& ec);
    };
}
