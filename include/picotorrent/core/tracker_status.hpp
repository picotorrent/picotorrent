#pragma once

#include <string>

namespace picotorrent
{
namespace core
{
    class tracker_status
    {
    public:
        tracker_status()
            : num_peers(-1),
            scrape_complete(-1),
            scrape_incomplete(-1)
        {
        }

        std::string message;
        int num_peers;
        int scrape_complete;
        int scrape_incomplete;
    };
}
}
