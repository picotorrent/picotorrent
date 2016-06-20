#include <picotorrent/client/ui/property_sheets/preferences/plugins_page.hpp>

#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/common/translator.hpp>
#include <picotorrent/extensibility/plugin_engine.hpp>

#include <windowsx.h>

#include <sstream>

using picotorrent::common::to_wstring;
using picotorrent::client::ui::property_sheets::preferences::plugins_page;
using picotorrent::extensibility::plugin_engine;

plugins_page::plugins_page(const std::shared_ptr<plugin_engine> &plugins)
    : plugins_(plugins)
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_PREFERENCES_PLUGINS);
    set_title(TR("plugins"));
}

void plugins_page::add_plugin(const std::string &name, const std::string &version)
{
    HWND hCombo = GetDlgItem(handle(), ID_PLUGINS_COMBO);

    std::stringstream ss;
    ss << name << " (" << version << ")";

    ComboBox_AddString(hCombo, to_wstring(ss.str()).c_str());
}

BOOL plugins_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

void plugins_page::on_init_dialog()
{
}
