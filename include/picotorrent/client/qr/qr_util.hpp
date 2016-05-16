#pragma once

#include <string>
#include <vector>

#include <picotorrent/client/qr/polynomial.hpp>

namespace picotorrent
{
namespace client
{
namespace qr
{
    class qr_code;

    class qr_util
    {
    public:
        static std::string get_jis_encoding();
        static std::vector<int> get_pattern_position(int type_number);
        static int get_max_length(int type_number, int mode, int error_correct_level);
        static polynomial get_error_correct_polynomial(int error_correct_length);
        static bool get_mask(int mask_pattern, int i, int j);
        static int get_lost_point(qr_code &qr);
        static int get_mode(const std::string &m);
        static int get_bch_type_info(int data);
        static int get_bch_type_number(int data);
        static int get_bch_digit(int data);

    private:
        qr_util() {}
    };
}
}
}
