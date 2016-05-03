#pragma once

#include <vector>

namespace picotorrent
{
namespace client
{
namespace qr
{
    class rs_block
    {
    public:
        rs_block(int total_count, int data_count);

        int get_data_count();
        int get_total_count();

        static std::vector<rs_block> get_rs_blocks(int type_number, int error_correct_level);

    private:
        static std::vector<int> get_rs_block_table(int type_number, int error_correct_level);

        int data_count_;
        int total_count_;
    };
}
}
}
