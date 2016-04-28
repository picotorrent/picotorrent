#include <picotorrent/client/qr/qr_code.hpp>

#include <cmath>

#include <picotorrent/client/qr/qr_util.hpp>

using picotorrent::client::qr::qr_code;
using picotorrent::client::qr::qr_util;

qr_code::qr_code()
{
    type_number_ = 1;

}

int qr_code::get_best_mask_pattern()
{
    int min_lost_point = 0;
    int pattern = 0;

    for (int i = 0; i < 8; i++)
    {
        make(true, i);
        int lost_point = qr_util::get_lost_point(*this);

        if (i == 0 || min_lost_point > lost_point)
        {
            min_lost_point = lost_point;
            pattern = i;
        }
    }

    return pattern;
}

int qr_code::get_module_count()
{
    return module_count_;
}

bool qr_code::is_dark(int row, int col)
{
    if (modules_[row][col].has_value)
    {
        return modules_[row][col].value;
    }

    return false;
}

void qr_code::make()
{
    make(false, get_best_mask_pattern());
}

void qr_code::make(bool test, int mask_pattern)
{
    module_count_ = type_number_ * 4 + 17;
    modules_ = std::vector<std::vector<module_val>>(module_count_, std::vector<module_val>(module_count_));

    setup_position_probe_pattern(0, 0);
    setup_position_probe_pattern(module_count_ - 7, 0);
    setup_position_probe_pattern(0, module_count_ - 7);

    setup_position_adjust_pattern();
    setup_timing_pattern();

    setup_type_info(test, mask_pattern);

    if (type_number_ >= 7)
    {
        setup_type_number(test);
    }

    std::vector<char> data = create_data(type_number_, error_correct_level_, qr_data_list_);
    map_data(data, mask_pattern);
}

void qr_code::map_data(const std::vector<char> &data, int mask_pattern)
{
    int inc = -1;
    int row = module_count_ - 1;
    int bit_index = 7;
    int byte_index = 0;

    for (int col = module_count_ - 1; col > 0; col -= 2)
    {
        if (col == 6) col--;

        while (true)
        {
            for (int c = 0; c < 2; c++)
            {
                if (!modules_[row][col - c].has_value)
                {
                    bool dark = false;

                    if (byte_index < data.size())
                    {
                        dark = ((data[byte_index] >> bit_index) & 1) == 1;
                    }

                    bool mask = qr_util::get_mask(mask_pattern, row, col - c);
                    if (mask) dark = !dark;

                    modules_[row][col - c].has_value = true;
                    modules_[row][col - c].value = dark;

                    bit_index--;

                    if (bit_index == -1)
                    {
                        byte_index++;
                        bit_index = 7;
                    }
                }

                row += inc;

                if (row < 0 || module_count_ <= row)
                {
                    row -= inc;
                    inc = -inc;
                    break;
                }
            }
        }
    }
}

void qr_code::setup_position_adjust_pattern()
{
    std::vector<int> pos = qr_util::get_pattern_position(type_number_);

    for (size_t i = 0; i < pos.size(); i++)
    {
        for (size_t j = 0; j < pos.size(); j++)
        {
            int row = pos[i];
            int col = pos[j];

            if (modules_[row][col].has_value)
            {
                continue;
            }

            for (int r = -2; r <= 2; r++)
            {
                for (int c = -2; c <= 2; c++)
                {
                    if (r == -2 || r == 2 || c == -2 || c == 2 || (r == 0 && c == 0))
                    {
                        modules_[row + r][col + c].has_value = true;
                        modules_[row + r][col + c].value = true;
                    }
                    else
                    {
                        modules_[row + r][col + c].has_value = true;
                        modules_[row + r][col + c].value = false;
                    }
                }
            }
        }
    }
}

void qr_code::setup_position_probe_pattern(int row, int col)
{
    for (int r = -1; r <= 7; r++)
    {
        for (int c = -1; c <= 7; c++)
        {
            if (row + r <= -1 || module_count_ <= row + r
                || col + c <= -1 || module_count_ <= col + c)
            {
                continue;
            }

            if ((0 <= r && r <= 6 && (c == 0 || c == 6))
                || (0 <= c && c <= 6 && (r == 0 || r == 6))
                || (2 <= r && r <= 4 && 2 <= c && c <= 4))
            {
                modules_[row + r][col + c].has_value = true;
                modules_[row + r][col + c].value = true;
            }
            else
            {
                modules_[row + r][col + c].has_value = true;
                modules_[row + r][col + c].value = false;
            }
        }
    }
}

void qr_code::setup_timing_pattern()
{
    for (int r = 8; r < module_count_ - 8; r++)
    {
        if (modules_[r][6].has_value)
        {
            continue;
        }

        modules_[r][6].has_value = true;
        modules_[r][6].value = (r % 2 == 0);
    }

    for (int c = 8; c < module_count_ - 8; c++)
    {
        if (modules_[6][c].has_value)
        {
            continue;
        }

        modules_[6][c].has_value = true;
        modules_[6][c].value = (c % 2 == 0);
    }
}

void qr_code::setup_type_info(bool test, int mask_pattern)
{
    int data = (error_correct_level_ << 3) | mask_pattern;
    int bits = qr_util::get_bch_type_info(data);

    for (int i = 0; i < 15; i++)
    {
        bool mod = (!test && ((bits >> i) & 1) == 1);

        if (i < 6)
        {
            modules_[i][8].has_value = true;
            modules_[i][8].value = mod;
        }
        else if (i < 8)
        {
            modules_[i + 1][8].has_value = true;
            modules_[i + 1][8].value = mod;
        }
        else
        {
            modules_[module_count_ - 15 + i][8].has_value = true;
            modules_[module_count_ - 15 + i][8].value = mod;
        }
    }

    for (int i = 0; i < 15; i++)
    {
        bool mod = (!test && ((bits >> i) & 1) == 1);

        if (i < 8)
        {
            modules_[8][module_count_ - i - 1].has_value = true;
            modules_[8][module_count_ - i - 1].value = mod;
        }
        else if (i < 9)
        {
            modules_[8][15 - i - 1 + 1].has_value = true;
            modules_[8][15 - i - 1 + 1].value = mod;
        }
        else
        {
            modules_[8][15 - i - 1].has_value = true;
            modules_[8][15 - i - 1].value = mod;
        }
    }

    modules_[module_count_ - 8][8].has_value = true;
    modules_[module_count_ - 8][8].value = !test;
}

void qr_code::setup_type_number(bool test)
{
    int bits = qr_util::get_bch_type_number(type_number_);

    for (int i = 0; i < 18; i++)
    {
        bool mod = (!test && ((bits >> i) & 1) == 1);
        modules_[i / 3][i % 3 + module_count_ - 8 - 3].has_value = true;
        modules_[i / 3][i % 3 + module_count_ - 8 - 3].value = mod;
    }

    for (int i = 0; i < 18; i++)
    {
        bool mod = (!test && ((bits >> i) & 1) == 1);
        modules_[i % 3 + module_count_ - 8 - 3][i / 3].has_value = true;
        modules_[i % 3 + module_count_ - 8 - 3][i / 3].value = mod;
    }
}
