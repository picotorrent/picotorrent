#pragma once

#include <vector>

namespace picotorrent
{
namespace client
{
namespace qr
{
    class bit_buffer
    {
    public:
        bit_buffer();

        std::vector<char> get_buffer();
        int get_length_in_bits();
        void put(int num, int length);
        void put(bool bit);

    private:
        std::vector<char> buffer_;
        int inclements_;
        int length_;
    };
}
}
}
