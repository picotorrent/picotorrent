#pragma once

#include <picotorrent/client/qr/qr_data.hpp>

namespace picotorrent
{
namespace client
{
namespace qr
{
    class qr_alpha_num : public qr_data
    {
    public:
        qr_alpha_num(const std::string &data);

        int get_length();
        void write(bit_buffer &buffer);

    private:
        static int get_code(char c);
    };
}
}
}
