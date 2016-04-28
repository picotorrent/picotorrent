#pragma once

#include <string>

namespace picotorrent
{
namespace client
{
namespace qr
{
    class bit_buffer;

    class qr_data
    {
    public:
        std::string get_data();
        virtual int get_length() = 0;
        int get_length_in_bits(int type);
        int get_mode();
        virtual void write(const bit_buffer &buffer) = 0;

    protected:
        qr_data(int mode, const std::string &data);

    private:
        int mode_;
        std::string data_;
    };
}
}
}
