#pragma once

#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>
#include <string>
#include <vector>

namespace picotorrent
{
namespace i18n
{
    struct translation;
}
namespace ui
{
namespace property_sheets
{
namespace preferences
{
    class general_page : public property_sheet_page
    {
    public:
        general_page();
        ~general_page();

        void add_languages(const std::vector<i18n::translation> &translations);
        int get_selected_language();
        void select_language(int langId);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();
    };
}
}
}
}
