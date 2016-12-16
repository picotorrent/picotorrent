#pragma once

#include <vector>

namespace Models { struct Tracker; }

namespace Commands
{
    struct RemoveTrackersCommand
    {
        std::vector<Models::Tracker> trackers;
    };
}
