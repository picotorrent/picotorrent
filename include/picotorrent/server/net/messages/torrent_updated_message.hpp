#pragma once

#include <memory>
#include <picotorrent/server/net/messages/message.hpp>

namespace picotorrent
{
namespace core
{
    class torrent;
}
namespace server
{
namespace net
{
namespace messages
{
    class torrent_updated_message : public message
    {
    public:
        torrent_updated_message(const std::shared_ptr<core::torrent>&);
        ~torrent_updated_message();

        void write_to(std::ostream &stream) const;

    private:
        std::shared_ptr<core::torrent> torrent_;
    };
}
}
}
}
