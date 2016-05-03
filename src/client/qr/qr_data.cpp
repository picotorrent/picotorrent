#include <picotorrent/client/qr/qr_data.hpp>

using picotorrent::client::qr::qr_data;

qr_data::qr_data(int mode, const std::string &data)
    : mode_(mode),
    data_(data)
{
}

std::string qr_data::get_data()
{
    return data_;
}

int qr_data::get_mode()
{
    return mode_;
}

int qr_data::get_length_in_bits(int type)
{
    if (1 <= type && type < 10)
    {
        // 1 - 9
        // TODO
        return 9;
    }
    else if (type < 27)
    {
        // 10 - 26
        // TODO
        return 11;
    }
    else if (type < 41)
    {
        // 27 - 40
        // TODO
        return 13;
    }

    throw std::exception("Unsupported");
}
