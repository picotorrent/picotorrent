#pragma once

#include <vector>

namespace Commands
{
    struct PrioritizeFilesCommand
    {
        std::vector<int> indices;
        uint8_t priority;
    };
}
