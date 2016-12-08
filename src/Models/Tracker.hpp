#pragma once

#include <chrono>
#include <string>

namespace Models
{
    struct Tracker
    {
        enum State
        {
            Unknown,
            NotWorking,
            Working,
            Updating
        };

        std::string url;
        State state;
        std::chrono::seconds nextUpdate;
        int downloaded;
        int complete;
        int incomplete;
    };
}
