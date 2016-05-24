#include <picotorrent/common/security/random_string_generator.hpp>

#include <algorithm>
#include <iterator>
#include <random>

using picotorrent::common::security::random_string_generator;

std::string random_string_generator::generate(int len)
{
    static const char alphabet[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    std::random_device rd;
    std::default_random_engine rng(rd());
    std::uniform_int_distribution<> dist(0, sizeof(alphabet) / sizeof(*alphabet) - 2);

    std::string val;
    std::generate_n(
        std::back_inserter(val),
        len,
        [&]()
    {
        return alphabet[dist(rng)];
    });

    return val;
}
