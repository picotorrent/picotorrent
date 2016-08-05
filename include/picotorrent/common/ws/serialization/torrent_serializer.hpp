#pragma once

#include <memory>

#include <picojson.hpp>

namespace picotorrent
{
namespace core
{
    class torrent;
}
namespace common
{
namespace ws
{
namespace serialization
{
    class torrent_serializer
    {
    public:
        static picojson::object serialize(const std::shared_ptr<core::torrent> &torrent);
    };
}
}
}
}
