#pragma once

#include <picotorrent/client/executor.hpp>

namespace picotorrent
{
namespace common
{
    class command_line;
}
namespace client
{
    class other_instance_executor : public executor
    {
    public:
        int run(const common::command_line &cmd);
    };
}
}
