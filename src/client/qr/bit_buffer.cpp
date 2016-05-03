#include <picotorrent/client/qr/bit_buffer.hpp>

using picotorrent::client::qr::bit_buffer;

bit_buffer::bit_buffer()
{
    inclements_ = 32;
    buffer_ = std::vector<char>(inclements_);
    length_ = 0;
}

std::vector<char> bit_buffer::get_buffer()
{
    return buffer_;
}

int bit_buffer::get_length_in_bits()
{
    return length_;
}

void bit_buffer::put(int num, int length)
{
    for (int i = 0; i < length; i++)
    {
        put(((num >> (length - i - 1)) & 1) == 1);
    }
}

void bit_buffer::put(bool bit)
{
    if (length_ == (int)buffer_.size() * 8)
    {
        buffer_.reserve(buffer_.size() + inclements_);
    }

    if (bit)
    {
        buffer_[length_ / 8] |= (0x80 >> (length_ % 8));
    }

    length_++;
}
