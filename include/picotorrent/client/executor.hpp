#pragma once

#include <memory>
#include <vector>

namespace picotorrent
{
namespace common
{
    class command_line;
}
namespace core
{
    class torrent;
}
namespace client
{
    class executor
    {
    public:
        virtual int run(const common::command_line &cmd) = 0;

        virtual void notification_available() { }
        virtual void torrent_added(const std::shared_ptr<core::torrent>&) { }
        virtual void torrent_finished(const std::shared_ptr<core::torrent>&) { }
        virtual void torrent_removed(const std::shared_ptr<core::torrent>&) { }
        virtual void torrent_updated(const std::vector<std::shared_ptr<core::torrent>>&) { }
    };
}
}
