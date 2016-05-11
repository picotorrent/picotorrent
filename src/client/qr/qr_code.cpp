#include <picotorrent/client/qr/qr_code.hpp>

#include <algorithm>
#include <cmath>

#include <picotorrent/client/qr/bit_buffer.hpp>
#include <picotorrent/client/qr/qr_8bit_byte.hpp>
#include <picotorrent/client/qr/qr_data.hpp>
#include <picotorrent/client/qr/qr_util.hpp>
#include <picotorrent/client/qr/rs_block.hpp>

#define PAD0 0xEC
#define PAD1 0x11

using picotorrent::client::qr::bit_buffer;
using picotorrent::client::qr::qr_8bit_byte;
using picotorrent::client::qr::qr_code;
using picotorrent::client::qr::qr_data;
using picotorrent::client::qr::qr_util;
using picotorrent::client::qr::rs_block;

qr_code::qr_code()
{
    type_number_ = 1;

}

void qr_code::set_error_correct_level(int error_correct_level)
{
    error_correct_level_ = error_correct_level;
}

void qr_code::set_type_number(int type_number)
{
    type_number_ = type_number;
}

void qr_code::add_data(const std::string &data)
{
    add_data(data, qr_util::get_mode(data));
}

void qr_code::add_data(const std::string &data, int mode)
{
    switch (mode)
    {
    case MODE_8BIT_BYTE:
        add_data(std::make_shared<qr_8bit_byte>(data));
        break;
    default:
        throw std::exception("invalid mode");
    }
}

void qr_code::add_data(const std::shared_ptr<qr_data> &data)
{
    qr_data_list_.push_back(data);
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

std::vector<char> qr_code::create_data(int type_number, int error_correct_level, std::vector<std::shared_ptr<qr_data>> &data)
{
    std::vector<rs_block> rs_blocks = rs_block::get_rs_blocks(type_number, error_correct_level);
    bit_buffer buffer;

    for (size_t i = 0; i < data.size(); i++)
    {
        std::shared_ptr<qr_data> &d = data[i];
        buffer.put(d->get_mode(), 4);
        buffer.put(d->get_length(), d->get_length_in_bits(type_number));
        d->write(buffer);
    }

    int total_data_count = 0;
    for (size_t i = 0; i < rs_blocks.size(); i++)
    {
        total_data_count += rs_blocks[i].get_data_count();
    }

    if (buffer.get_length_in_bits() > total_data_count * 8)
    {
        throw std::exception("code length overflow");
    }

    if (buffer.get_length_in_bits() + 4 <= total_data_count * 8)
    {
        buffer.put(0, 4);
    }

    while (buffer.get_length_in_bits() % 8 != 0)
    {
        buffer.put(false);
    }

    while (true)
    {
        if (buffer.get_length_in_bits() >= total_data_count * 8)
        {
            break;
        }

        buffer.put(PAD0, 8);

        if (buffer.get_length_in_bits() >= total_data_count * 8)
        {
            break;
        }

        buffer.put(PAD1, 8);
    }

    return create_bytes(buffer, rs_blocks);
}

std::vector<char> qr_code::create_bytes(bit_buffer &buffer, std::vector<rs_block> &blocks)
{
    int offset = 0;

    int max_dc_count = 0;
    int max_ec_count = 0;

    std::vector<std::vector<int>> dc_data(blocks.size(), std::vector<int>());
    std::vector<std::vector<int>> ec_data(blocks.size(), std::vector<int>());

    for (size_t r = 0; r < blocks.size(); r++)
    {
        int dc_count = blocks[r].get_data_count();
        int ec_count = blocks[r].get_total_count() - dc_count;

        max_dc_count = std::max(max_dc_count, dc_count);
        max_ec_count = std::max(max_ec_count, ec_count);

        dc_data[r] = std::vector<int>(dc_count);

        for (size_t i = 0; i < dc_data[r].size(); i++)
        {
            dc_data[r][i] = 0xff & buffer.get_buffer()[i + offset];
        }

        offset += dc_count;

        polynomial rs_poly = qr_util::get_error_correct_polynomial(ec_count);
        polynomial raw_poly(dc_data[r], rs_poly.get_length() - 1);
        polynomial mod_poly = raw_poly.mod(rs_poly);

        ec_data[r] = std::vector<int>(rs_poly.get_length() - 1);

        for (size_t i = 0; i < ec_data[r].size(); i++)
        {
            int mod_index = (int)(i + mod_poly.get_length() - ec_data[r].size());
            ec_data[r][i] = (mod_index >= 0) ? mod_poly.get(mod_index) : 0;
        }
    }

    int total_code_count = 0;
    for (size_t i = 0; i < blocks.size(); i++)
    {
        total_code_count += blocks[i].get_total_count();
    }

    std::vector<char> data(total_code_count);
    int index = 0;

    for (int i = 0; i < max_dc_count; i++)
    {
        for (size_t r = 0; r < blocks.size(); r++)
        {
            if (i < dc_data[r].size())
            {
                data[index++] = (char)dc_data[r][i];
            }
        }
    }

    for (int i = 0; i < max_ec_count; i++)
    {
        for (size_t r = 0; r < blocks.size(); r++)
        {
            if (i < ec_data[r].size())
            {
                data[index++] = (char)ec_data[r][i];
            }
        }
    }

    return data;
}
