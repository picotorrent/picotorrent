#pragma once

#include <ostream>

namespace picotorrent
{
namespace server
{
namespace net
{
namespace messages
{
    class message
    {
    public:
        virtual void write_to(std::ostream &stream) const = 0;
    };
}
}
}
}
