#pragma once

#include <picotorrent/client/qr/qr_data.hpp>

namespace picotorrent
{
namespace client
{
namespace qr
{
    class qr_8bit_byte : public qr_data
    {
    public:
        qr_8bit_byte(const std::string &data);

        int get_length();
        void write(bit_buffer &buffer);
    };
}
}
}
