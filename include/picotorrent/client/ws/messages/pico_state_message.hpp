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
    class pico_state_message
    {
    public:
        pico_state_message(const std::vector<std::shared_ptr<core::torrent>> &torrents);
        std::string serialize();

    private:
        std::vector<std::shared_ptr<core::torrent>> torrents_;
    };
}
}
}
}
