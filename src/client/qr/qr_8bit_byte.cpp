#include <picotorrent/client/qr/qr_8bit_byte.hpp>

#include <iterator>

#include <picotorrent/client/qr/bit_buffer.hpp>

using picotorrent::client::qr::bit_buffer;
using picotorrent::client::qr::qr_8bit_byte;

qr_8bit_byte::qr_8bit_byte(const std::string &data)
    : qr_data(1<<2, data)
{
}

int qr_8bit_byte::get_length()
{
    return (int)get_data().size();
}

void qr_8bit_byte::write(bit_buffer &buffer)
{
    std::string s = get_data();
    std::vector<char> data;
    std::copy(s.begin(), s.end(), std::back_inserter(data));

    for (char c : data)
    {
        buffer.put(c, 8);
    }
}
