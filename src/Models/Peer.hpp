#pragma once

#include <string>

namespace Models
{
    struct Peer
    {
        bool operator==(const Peer& other)
        {
            return index == other.index;
        }

        bool operator!=(const Peer& other)
        {
            return !(*this == other);
        }

        int index;
    };
}
