#include <picotorrent/client/ui/property_sheets/preferences/plugins_page.hpp>

#include <picotorrent/client/ui/resources.hpp>
#include <picotorrent/client/ui/scaler.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/common/translator.hpp>
#include <picotorrent/extensibility/plugin_engine.hpp>
#include <windowsx.h>

#include <sstream>

using picotorrent::common::to_wstring;
using picotorrent::client::ui::property_sheets::preferences::plugins_page;
using picotorrent::extensibility::plugin_engine;

plugins_page::plugins_page()
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

picotorrent::core::signals::signal_connector<void, int>& plugins_page::on_plugin_changed()
{
    return plugin_changed_;
}

void plugins_page::select_plugin(int index)
{
    HWND ctl = GetDlgItem(handle(), ID_PLUGINS_COMBO);
    ComboBox_SetCurSel(ctl, index);
}

void plugins_page::set_plugin_config_hwnd(HWND hWnd)
{
    SetParent(hWnd, handle());

    RECT prc;
    GetWindowRect(handle(), &prc);

    int padding = scaler::x(7);
    int paddingY = scaler::y(50);

    SetWindowPos(
        hWnd,
        NULL,
        padding,
        paddingY,
        prc.right - prc.left - padding - padding,
        prc.bottom - prc.top - paddingY - padding,
        SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL plugins_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    switch (uCtrlId)
    {
    case ID_PLUGINS_COMBO:
    {
        if (HIWORD(wParam) == CBN_SELENDOK)
        {
            HWND ctl = GetDlgItem(handle(), ID_PLUGINS_COMBO);
            int index = ComboBox_GetCurSel(ctl);
            plugin_changed_.emit(index);
        }
        break;
    }
    }
    return FALSE;
}

void plugins_page::on_init_dialog()
{
}
