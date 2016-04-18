#pragma once

#include <memory>
#include <string>

namespace picotorrent
{
namespace core
{
    class torrent;
}
namespace client
{
namespace ws
{
namespace messages
{
    class torrent_added_message
    {
    public:
        torrent_added_message(const std::shared_ptr<core::torrent> &torrent);
        std::string serialize();

    private:
        std::shared_ptr<core::torrent> torrent_;
    };
}
}
}
}
