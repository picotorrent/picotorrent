#pragma once

#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>
#include <string>
#include <vector>

namespace picotorrent
{
namespace client
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
        void add_start_position(int id, const std::wstring &name);
        bool get_autostart_checked();
        int get_selected_language();
        int get_selected_start_position();
        void select_language(int langId);
        void select_start_position(int posId);
        void set_autostart_checked(bool value);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();
    };
}
}
}
}
}
