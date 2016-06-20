#pragma once

#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>
#include <memory>
#include <string>

namespace picotorrent
{
namespace extensibility
{
    class plugin_engine;
}
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
        plugins_page(const std::shared_ptr<extensibility::plugin_engine> &plugins);

        void add_plugin(const std::string &name, const std::string &version);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();

    private:
        std::shared_ptr<extensibility::plugin_engine> plugins_;
    };
}
}
}
}
}
