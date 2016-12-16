#pragma once

#include <string>

namespace Models
{
    struct TorrentFile
    {
        bool operator==(const TorrentFile& other)
        {
            return index == other.index;
        }

        bool operator!=(const TorrentFile& other)
        {
            return !(*this == other);
        }

        int index;
        std::wstring name;
        int64_t size;
        float progress;
        uint8_t priority;
    };
}
