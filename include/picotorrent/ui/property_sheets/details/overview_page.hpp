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
    class overview_page : public property_sheet_page
    {
    public:
        overview_page();

        int dl_limit();
        void dl_limit(int limit);
        int ul_limit();
        void ul_limit(int limit);
        int max_connections();
        void max_connections(int max);
        int max_uploads();
        void max_uploads(int max);
        bool sequential_download();
        void sequential_download(bool val);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();

    private:
        void check_changed(HWND hDlg, UINT uCtrlId, UINT uCommand);
    };
}
}
}
}
