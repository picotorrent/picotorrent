#include <picotorrent/client/qr/qr_alpha_num.hpp>

#include <iterator>

#include <picotorrent/client/qr/bit_buffer.hpp>

using picotorrent::client::qr::bit_buffer;
using picotorrent::client::qr::qr_alpha_num;

qr_alpha_num::qr_alpha_num(const std::string &data)
    : qr_data(1<<1, data)
{
}

int qr_alpha_num::get_length()
{
    return (int)get_data().size();
}

void qr_alpha_num::write(bit_buffer &buffer)
{
    std::string s = get_data();
    std::vector<char> data;
    std::copy(s.begin(), s.end(), std::back_inserter(data));

    int i = 0;

    while (i + 1 < data.size())
    {
        buffer.put(get_code(data[i]) * 45 + get_code(data[i + 1]), 11);
        i += 2;
    }

    if (i < data.size())
    {
        buffer.put(get_code(data[i]), 6);
    }
}

int qr_alpha_num::get_code(char c)
{
    if ('0' <= c && c <= '9')
    {
        return c - '0';
    }
    else if ('A' <= c && c <= 'Z')
    {
        return c - 'A' + 10;
    }
    else
    {
        switch (c) {
        case ' ': return 36;
        case '$': return 37;
        case '%': return 38;
        case '*': return 39;
        case '+': return 40;
        case '-': return 41;
        case '.': return 42;
        case '/': return 43;
        case ':': return 44;
        default:
            throw std::exception("illegal char :" + c);
        }
    }
}
