#pragma once

#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>
#include <string>

namespace picotorrent
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
        
        std::wstring downloads_path();
        bool prompt_for_save_path();
        void set_downloads_path(const std::wstring &path);
        void set_prompt_for_save_path(bool prompt);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();
    };
}
}
}
}
