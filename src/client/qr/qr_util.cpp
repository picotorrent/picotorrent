#include <picotorrent/client/qr/qr_util.hpp>

#include <picotorrent/client/qr/qr_code.hpp>
#include <picotorrent/client/qr/qr_math.hpp>

using picotorrent::client::qr::polynomial;
using picotorrent::client::qr::qr_code;
using picotorrent::client::qr::qr_math;
using picotorrent::client::qr::qr_util;

int G15 = (1 << 10) | (1 << 8) | (1 << 5) | (1 << 4) | (1 << 2) | (1 << 1) | (1 << 0);
int G18 = (1 << 12) | (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8) | (1 << 5) | (1 << 2) | (1 << 0);
int G15_MASK = (1 << 14) | (1 << 12) | (1 << 10) | (1 << 4) | (1 << 1);

std::vector<std::vector<int>> PATTERN_POSITION_TABLE = {
    {},
    { 6, 18 },
    { 6, 22 },
    { 6, 26 },
    { 6, 30 },
    { 6, 34 },
    { 6, 22, 38 },
    { 6, 24, 42 },
    { 6, 26, 46 },
    { 6, 28, 50 },
    { 6, 30, 54 },
    { 6, 32, 58 },
    { 6, 34, 62 },
    { 6, 26, 46, 66 },
    { 6, 26, 48, 70 },
    { 6, 26, 50, 74 },
    { 6, 30, 54, 78 },
    { 6, 30, 56, 82 },
    { 6, 30, 58, 86 },
    { 6, 34, 62, 90 },
    { 6, 28, 50, 72, 94 },
    { 6, 26, 50, 74, 98 },
    { 6, 30, 54, 78, 102 },
    { 6, 28, 54, 80, 106 },
    { 6, 32, 58, 84, 110 },
    { 6, 30, 58, 86, 114 },
    { 6, 34, 62, 90, 118 },
    { 6, 26, 50, 74, 98, 122 },
    { 6, 30, 54, 78, 102, 126 },
    { 6, 26, 52, 78, 104, 130 },
    { 6, 30, 56, 82, 108, 134 },
    { 6, 34, 60, 86, 112, 138 },
    { 6, 30, 58, 86, 114, 142 },
    { 6, 34, 62, 90, 118, 146 },
    { 6, 30, 54, 78, 102, 126, 150 },
    { 6, 24, 50, 76, 102, 128, 154 },
    { 6, 28, 54, 80, 106, 132, 158 },
    { 6, 32, 58, 84, 110, 136, 162 },
    { 6, 26, 54, 82, 110, 138, 166 },
    { 6, 30, 58, 86, 114, 142, 170 }
};

int qr_util::get_bch_digit(int data)
{
    int digit = 0;

    while (data != 0)
    {
        digit++;
        data >>= 1;
    }

    return digit;
}

int qr_util::get_bch_type_info(int data)
{
    int d = data << 10;

    while (get_bch_digit(d) - get_bch_digit(G15) >= 0)
    {
        d ^= (G15 << (get_bch_digit(d) - get_bch_digit(G15)));
    }

    return ((data << 10) | d) ^ G15_MASK;
}

int qr_util::get_bch_type_number(int data)
{
    int d = data << 12;

    while (get_bch_digit(d) - get_bch_digit(G18) >= 0)
    {
        d ^= (G18 << (get_bch_digit(d) - get_bch_digit(G18)));
    }

    return (data << 10) | d;
}

polynomial qr_util::get_error_correct_polynomial(int error_correct_length)
{
    polynomial a({ 1 });

    for (int i = 0; i < error_correct_length; i++)
    {
        a = a.multiply(polynomial({ 1, qr_math::gexp(i) }));
    }

    return a;
}

int qr_util::get_lost_point(qr_code &qr)
{
    int module_count = qr.get_module_count();
    int lost_point = 0;

    // LEVEL 1
    for (int row = 0; row < module_count; row++)
    {
        for (int col = 0; col < module_count; col++)
        {
            int same_count = 0;
            bool dark = qr.is_dark(row, col);

            for (int r = -1; r < 2; r++)
            {
                if (row + r < 0 || module_count <= row + r)
                {
                    continue;
                }

                for (int c = -1; c < 2; c++)
                {
                    if (col + c < 0 || module_count <= col + c)
                    {
                        continue;
                    }

                    if (r == 0 || c == 0)
                    {
                        continue;
                    }

                    if (dark == qr.is_dark(row + r, col + c))
                    {
                        same_count += 1;
                    }
                }
            }

            if (same_count > 5)
            {
                lost_point += (3 + same_count - 5);
            }
        }
    }

    // LEVEL 2
    for (int row = 0; row < module_count - 1; row++)
    {
        for (int col = 0; col < module_count - 1; col++)
        {
            int count = 0;
            if (qr.is_dark(row, col)) count += 1;
            if (qr.is_dark(row + 1, col)) count += 1;
            if (qr.is_dark(row, col + 1)) count += 1;
            if (qr.is_dark(row + 1, col + 1)) count += 1;

            if (count == 0 || count == 4)
            {
                lost_point += 3;
            }
        }
    }

    // LEVEL 3
    for (int row = 0; row < module_count; row++)
    {
        for (int col = 0; col < module_count - 6; col++)
        {
            if (qr.is_dark(row, col)
                && !qr.is_dark(row, col + 1)
                && qr.is_dark(row, col + 2)
                && qr.is_dark(row, col + 3)
                && qr.is_dark(row, col + 4)
                && !qr.is_dark(row, col + 5)
                && qr.is_dark(row, col + 6))
            {
                lost_point += 40;
            }
        }
    }

    for (int col = 0; col < module_count; col++)
    {
        for (int row = 0; row < module_count - 6; row++)
        {
            if (qr.is_dark(row, col)
                && !qr.is_dark(row + 1, col)
                && qr.is_dark(row + 2, col)
                && qr.is_dark(row + 3, col)
                && qr.is_dark(row + 4, col)
                && !qr.is_dark(row + 5, col)
                && qr.is_dark(row + 6, col))
            {
                lost_point += 40;
            }
        }
    }

    // LEVEL 4
    int dark_count = 0;
    for (int row = 0; row < module_count; row++)
    {
        for (int col = 0; col < module_count; col++)
        {
            if (qr.is_dark(row, col)) dark_count += 1;
        }
    }

    int ratio = std::abs(100 * dark_count / module_count / module_count - 50) / 5;
    lost_point += ratio * 10;

    return lost_point;
}

bool qr_util::get_mask(int mask_pattern, int i, int j)
{
    switch (mask_pattern)
    {
    case 0: return (i + j) % 2 == 0;
    case 1: return i % 2 == 0;
    case 2: return j % 3 == 0;
    case 3: return (i + j) % 3 == 0;
    case 4: return (i / 2 + j / 3) % 2 == 0;
    case 5: return (i * j) % 2 + (i * j) % 3 == 0;
    case 6: return ((i * j) % 2 + (i * j) % 3) % 2 == 0;
    case 7: return ((i * j) % 3 + (i + j) % 2) % 2 == 0;
    }

    throw std::exception("invalid mask");
}

int qr_util::get_mode(const std::string &m)
{
    return 1 << 2;
}

std::vector<int> qr_util::get_pattern_position(int type_number)
{
    return PATTERN_POSITION_TABLE[type_number - 1];
}
