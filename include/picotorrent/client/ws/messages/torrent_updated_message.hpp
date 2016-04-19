#pragma once

#include <memory>
#include <string>
#include <vector>

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
    class torrent_updated_message
    {
    public:
        torrent_updated_message(const std::vector<std::shared_ptr<core::torrent>> &torrents);
        std::string serialize();

    private:
        std::vector<std::shared_ptr<core::torrent>> torrents_;
    };
}
}
}
}
