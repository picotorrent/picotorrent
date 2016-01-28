#pragma once

#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>
#include <string>
#include <vector>

#include <commctrl.h>
#include <uxtheme.h>

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

        void add_file(const std::wstring &name, uint64_t size, float progress);
        void refresh();
        void update_file_progress(int index, float progress);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();
        bool on_notify(HWND hDlg, LPNMHDR nmhdr, LRESULT &lResult);

    private:
        void handle_draw_progress(LPNMLVCUSTOMDRAW lpCustomDraw, LRESULT &lResult);

        std::vector<float> files_progress_;
        HTHEME progress_theme_;
        HWND progress_;
    };
}
}
}
}
