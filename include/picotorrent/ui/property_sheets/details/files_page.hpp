#pragma once

#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>
#include <string>

namespace picotorrent
{
namespace ui
{
namespace property_sheets
{
namespace details
{
    class files_page : public property_sheet_page
    {
    public:
        files_page();

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init();
    };
}
}
}
}
