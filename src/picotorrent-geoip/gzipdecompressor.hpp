#pragma once

#include <vector>

namespace pt
{
    class GZipDecompressor
    {
    public:
        std::vector<char> decompress(std::vector<char> const& data);
    };
}
