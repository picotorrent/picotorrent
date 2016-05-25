#pragma once

#include <memory>

namespace boost { namespace asio { class io_service; } }

namespace picotorrent
{
namespace server
{
namespace hosting
{
    class console_host
    {
    public:
        int run(const std::shared_ptr<boost::asio::io_service> &io);
    };
}
}
}
