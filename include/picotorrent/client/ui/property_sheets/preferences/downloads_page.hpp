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
namespace preferences
{
    class downloads_page : public property_sheet_page
    {
    public:
        downloads_page();
        
        std::string downloads_path();
        int download_rate();
        bool prompt_for_save_path();
        int upload_rate();
        void set_downloads_path(const std::string &path);
        void set_download_rate(int dl_rate);
        void set_prompt_for_save_path(bool prompt);
        void set_upload_rate(int ul_rate);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();
    };
}
}
}
}
}
