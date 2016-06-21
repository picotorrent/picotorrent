#pragma once

#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>
#include <memory>
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
    class plugins_page : public property_sheet_page
    {
    public:
        plugins_page();

        void add_plugin(const std::string &name, const std::string &version);
        core::signals::signal_connector<void, int>& on_plugin_changed();
        void select_plugin(int index);
        void set_dirty();
        void set_plugin_config_hwnd(HWND hWnd);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();

    private:
        core::signals::signal<void, int> plugin_changed_;
    };
}
}
}
}
}
