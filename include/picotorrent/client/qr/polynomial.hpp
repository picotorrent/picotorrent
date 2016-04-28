#pragma once

#include <string>
#include <vector>

namespace picotorrent
{
namespace client
{
namespace qr
{
    class polynomial
    {
    public:
        polynomial(const std::vector<int> &num);
        polynomial(const std::vector<int> &num, int shift);

        int get(int index);
        int get_length();
        polynomial multiply(const polynomial &other);
        polynomial mod(const polynomial &other);
        std::string to_string();
        std::string to_log_string();
    };
}
}
}
