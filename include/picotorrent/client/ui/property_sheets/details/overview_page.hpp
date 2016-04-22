#pragma once

#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>
#include <string>

namespace picotorrent
{
namespace client
{
namespace ui
{
namespace property_sheets
{
namespace details
{
    class overview_page : public property_sheet_page
    {
    public:
        overview_page();

        void set_piece_info(int count, int have, int length);
        void set_ratio(float ratio);
        void set_total_download(int64_t dl);
        void set_total_upload(int64_t ul);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();

    private:
        std::string bytes_to_string(int64_t b);
    };
}
}
}
}
}
