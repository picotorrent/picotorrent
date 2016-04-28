#pragma once

#include <string>
#include <vector>

#include <picotorrent/client/qr/qr_data.hpp>

namespace picotorrent
{
namespace client
{
namespace qr
{
    class bit_buffer;
    class qr_8bit_byte;
    class rs_block;

    class qr_code
    {
    public:
        qr_code();

        int get_type_number();
        void set_type_number(int type_number);
        int get_error_correct_level();
        void set_error_correct_level(int error_correct_level);
        void add_data(const std::string &data);
        void add_data(const std::string &data, int mode);
        void clear_data();
        bool is_dark(int row, int col);
        int get_module_count();
        void make();

        static std::vector<char> create_data(int type_number, int error_correct_level, const std::vector<qr_data> &data);

    protected:
        void add_data(const qr_data &data);
        int get_data_count();
        qr_data& get_data(int index);

    private:
        struct module_val
        {
            bool value;
            bool has_value;
        };

        int get_best_mask_pattern();
        void make(bool test, int mask_pattern);
        void map_data(const std::vector<char> &data, int mask_pattern);
        void setup_position_adjust_pattern();
        void setup_position_probe_pattern(int row, int col);
        void setup_timing_pattern();
        void setup_type_number(bool test);
        void setup_type_info(bool test, int mask_pattern);

        static std::vector<char> create_bytes(const bit_buffer &buffer, const std::vector<rs_block> &blocks);

        int type_number_;
        std::vector<std::vector<module_val>> modules_;
        int module_count_;
        int error_correct_level_;
        std::vector<qr_data> qr_data_list_;
    };
}
}
}
