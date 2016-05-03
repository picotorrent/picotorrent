#include <picotorrent/client/qr/rs_block.hpp>

using picotorrent::client::qr::rs_block;

std::vector<std::vector<int>> RS_BLOCK_TABLE = {

    // L
    // M
    // Q
    // H

    // 1
    { 1, 26, 19 },
    { 1, 26, 16 },
    { 1, 26, 13 },
    { 1, 26, 9 },

    // 2
    { 1, 44, 34 },
    { 1, 44, 28 },
    { 1, 44, 22 },
    { 1, 44, 16 },

    // 3
    { 1, 70, 55 },
    { 1, 70, 44 },
    { 2, 35, 17 },
    { 2, 35, 13 },

    // 4		
    { 1, 100, 80 },
    { 2, 50, 32 },
    { 2, 50, 24 },
    { 4, 25, 9 },

    // 5
    { 1, 134, 108 },
    { 2, 67, 43 },
    { 2, 33, 15, 2, 34, 16 },
    { 2, 33, 11, 2, 34, 12 },

    // 6
    { 2, 86, 68 },
    { 4, 43, 27 },
    { 4, 43, 19 },
    { 4, 43, 15 },

    // 7		
    { 2, 98, 78 },
    { 4, 49, 31 },
    { 2, 32, 14, 4, 33, 15 },
    { 4, 39, 13, 1, 40, 14 },

    // 8
    { 2, 121, 97 },
    { 2, 60, 38, 2, 61, 39 },
    { 4, 40, 18, 2, 41, 19 },
    { 4, 40, 14, 2, 41, 15 },

    // 9
    { 2, 146, 116 },
    { 3, 58, 36, 2, 59, 37 },
    { 4, 36, 16, 4, 37, 17 },
    { 4, 36, 12, 4, 37, 13 },

    // 10		
    { 2, 86, 68, 2, 87, 69 },
    { 4, 69, 43, 1, 70, 44 },
    { 6, 43, 19, 2, 44, 20 },
    { 6, 43, 15, 2, 44, 16 }

};

rs_block::rs_block(int total_count, int data_count)
    : total_count_(total_count),
    data_count_(data_count)
{
}

int rs_block::get_data_count()
{
    return data_count_;
}

int rs_block::get_total_count()
{
    return total_count_;
}

std::vector<rs_block> rs_block::get_rs_blocks(int type_number, int error_correct_level)
{
    std::vector<int> block = get_rs_block_table(type_number, error_correct_level);
    int length = (int)block.size() / 3;

    std::vector<rs_block> list;

    for (int i = 0; i < length; i++)
    {
        int count = block[i * 3 + 0];
        int total_count = block[i * 3 + 1];
        int data_count = block[i * 3 + 2];

        for (int j = 0; j < count; j++)
        {
            list.push_back(rs_block(total_count, data_count));
        }
    }

    return list;
}

std::vector<int> rs_block::get_rs_block_table(int type_number, int error_correct_level)
{
    switch (error_correct_level)
    {
    case 1: return RS_BLOCK_TABLE[(type_number - 1) * 4 + 0];
    case 0: return RS_BLOCK_TABLE[(type_number - 1) * 4 + 1];
    case 3: return RS_BLOCK_TABLE[(type_number - 1) * 4 + 2];
    case 2: return RS_BLOCK_TABLE[(type_number - 1) * 4 + 3];
    }

    throw std::exception("invalid error level");
}
