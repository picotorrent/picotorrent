#pragma once

#include <string>

namespace picotorrent
{
namespace client
{
namespace ws
{
namespace messages
{
    class torrent_removed_message
    {
    public:
        torrent_removed_message(const std::string &hash);
        std::string serialize();

    private:
        std::string hash_;
    };
}
}
}
}
