#include <picotorrent/client/ui/property_sheets/preferences/connection_page.hpp>

#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>

#include <windowsx.h>
#include <commctrl.h>
#include <iostream>
#include <sstream>
#include <strsafe.h>

using picotorrent::client::ui::property_sheets::preferences::connection_page;

connection_page::connection_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(IDD_PREFERENCES_CONNECTION);
    set_title(TR("connection"));
}

void connection_page::add_proxy_type(const std::string &name, int type)
{
    HWND ctl = GetDlgItem(handle(), ID_PROXY_TYPE);

    int index = ComboBox_AddString(ctl, to_wstring(name).c_str());
    ComboBox_SetItemData(ctl, index, type);
}

std::vector<std::pair<std::string, int>> connection_page::get_listen_interfaces()
{
    std::vector<std::pair<std::string, int>> result;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    std::string l = get_dlg_item_text(ID_LISTEN_INTERFACES);

    while ((pos = l.find('\n', prev)) != std::string::npos)
    {
        std::string net_addr = trim(l.substr(prev, pos - prev));
        size_t idx = net_addr.find_last_of(":");

        if (idx == std::string::npos)
        {
            continue;
        }

        std::string addr = net_addr.substr(0, idx);
        int port = std::stoi(net_addr.substr(idx + 1));

        result.push_back({ addr,port });
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    std::string last_net_addr = trim(l.substr(prev, pos - prev));
    size_t lidx = last_net_addr.find_last_of(":");

    if (lidx != std::string::npos)
    {
        std::string addr = last_net_addr.substr(0, lidx);
        int port = std::stoi(last_net_addr.substr(lidx + 1));
        result.push_back({ addr,port });
    }

    return result;
}

int connection_page::get_proxy_type()
{
    HWND ctl = GetDlgItem(handle(), ID_PROXY_TYPE);
    int idx = ComboBox_GetCurSel(ctl);

    return (int)ComboBox_GetItemData(ctl, idx);
}

std::string connection_page::get_proxy_host()
{
    return get_dlg_item_text(ID_PROXY_HOST);
}

int connection_page::get_proxy_port()
{
    std::string text = get_dlg_item_text(ID_PROXY_PORT);
    if (text.empty()) { return -1; }
    return std::stoi(text);
}

std::string connection_page::get_proxy_username()
{
    return get_dlg_item_text(ID_PROXY_USERNAME);
}

std::string connection_page::get_proxy_password()
{
    return get_dlg_item_text(ID_PROXY_PASSWORD);
}

bool connection_page::get_proxy_force_checked()
{
    return is_checked(ID_PROXY_FORCE);
}

bool connection_page::get_proxy_hostnames_checked()
{
    return is_checked(ID_PROXY_HOSTNAMES);
}

bool connection_page::get_proxy_peers_checked()
{
    return is_checked(ID_PROXY_PEERS);
}

bool connection_page::get_proxy_trackers_checked()
{
    return is_checked(ID_PROXY_TRACKERS);
}

void connection_page::set_listen_interfaces(const std::vector<std::pair<std::string, int>> &interfaces)
{
    std::stringstream ss;

    for (auto &p : interfaces)
    {
        ss << p.first << ":" << p.second << "\r\n";
    }

    set_dlg_item_text(ID_LISTEN_INTERFACES, ss.str());
}

void connection_page::set_proxy_force_enabled(bool enabled)
{
    enable_window(ID_PROXY_FORCE, enabled);
}

void connection_page::set_proxy_hostnames_enabled(bool enabled)
{
    enable_window(ID_PROXY_HOSTNAMES, enabled);
}

void connection_page::set_proxy_peers_enabled(bool enabled)
{
    enable_window(ID_PROXY_PEERS, enabled);
}

void connection_page::set_proxy_trackers_enabled(bool enabled)
{
    enable_window(ID_PROXY_TRACKERS, enabled);
}

void connection_page::set_proxy_host(const std::string &value)
{
    set_dlg_item_text(ID_PROXY_HOST, value);
}

void connection_page::set_proxy_port(const std::string &value)
{
    set_dlg_item_text(ID_PROXY_PORT, value);
}

void connection_page::set_proxy_username(const std::string &value)
{
    set_dlg_item_text(ID_PROXY_USERNAME, value);
}

void connection_page::set_proxy_password(const std::string &value)
{
    set_dlg_item_text(ID_PROXY_PASSWORD, value);
}

void connection_page::set_proxy_force_checked(bool checked)
{
    CheckDlgButton(handle(), ID_PROXY_FORCE, checked ? BST_CHECKED : BST_UNCHECKED);
}

void connection_page::set_proxy_hostnames_checked(bool checked)
{
    CheckDlgButton(handle(), ID_PROXY_HOSTNAMES, checked ? BST_CHECKED : BST_UNCHECKED);
}

void connection_page::set_proxy_peers_checked(bool checked)
{
    CheckDlgButton(handle(), ID_PROXY_PEERS, checked ? BST_CHECKED : BST_UNCHECKED);
}

void connection_page::set_proxy_trackers_checked(bool checked)
{
    CheckDlgButton(handle(), ID_PROXY_TRACKERS, checked ? BST_CHECKED : BST_UNCHECKED);
}

void connection_page::set_proxy_host_enabled(bool enabled)
{
    enable_window(ID_PROXY_HOST, enabled);
}

void connection_page::set_proxy_port_enabled(bool enabled)
{
    enable_window(ID_PROXY_PORT, enabled);
}

void connection_page::set_proxy_username_enabled(bool enabled)
{
    enable_window(ID_PROXY_USERNAME, enabled);
}

void connection_page::set_proxy_password_enabled(bool enabled)
{
    enable_window(ID_PROXY_PASSWORD, enabled);
}

void connection_page::set_proxy_type(int type)
{
    HWND ctl = GetDlgItem(handle(), ID_PROXY_TYPE);

    for (int i = 0; i < ComboBox_GetCount(ctl); i++)
    {
        LRESULT data = ComboBox_GetItemData(ctl, i);

        if (data == type)
        {
            ComboBox_SetCurSel(ctl, i);
            break;
        }
    }
}

void connection_page::set_proxy_type_changed_callback(const std::function<void(int)> &callback)
{
    proxy_type_changed_cb_ = callback;
}

BOOL connection_page::on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam)
{
    if (!is_initializing())
    {
        check_changed(hDlg, uCtrlId, HIWORD(wParam));
    }

    switch (uCtrlId)
    {
    case ID_PROXY_TYPE:
    {
        if (HIWORD(wParam) == CBN_SELENDOK && proxy_type_changed_cb_)
        {
            HWND ctl = GetDlgItem(handle(), ID_PROXY_TYPE);
            int index = ComboBox_GetCurSel(ctl);
            int type = (int)ComboBox_GetItemData(ctl, index);

            proxy_type_changed_cb_(type);
        }
        break;
    }

    case ID_PROXY_FORCE:
    case ID_PROXY_HOSTNAMES:
    case ID_PROXY_PEERS:
    case ID_PROXY_TRACKERS:
    {
        bool checked = IsDlgButtonChecked(hDlg, uCtrlId) == BST_CHECKED;
        CheckDlgButton(hDlg, uCtrlId, checked ? BST_UNCHECKED : BST_CHECKED);
        break;
    }
    }
    return FALSE;
}

void connection_page::on_init_dialog()
{
    set_dlg_item_text(ID_LISTEN_INTERFACE_GROUP, TR("listen_interface"));
    set_dlg_item_text(ID_PROXY_GROUP, TR("proxy"));
    set_dlg_item_text(ID_TYPE_TEXT, TR("type"));
    set_dlg_item_text(ID_HOST_TEXT, TR("host"));
    set_dlg_item_text(ID_PROXY_PORT_TEXT, TR("port"));
    set_dlg_item_text(ID_PROXY_USERNAME_TEXT, TR("username"));
    set_dlg_item_text(ID_PROXY_PASSWORD_TEXT, TR("password"));
    set_dlg_item_text(ID_PROXY_FORCE, TR("force_proxy"));
    set_dlg_item_text(ID_PROXY_HOSTNAMES, TR("proxy_hostnames"));
    set_dlg_item_text(ID_PROXY_PEERS, TR("proxy_peer_connections"));
    set_dlg_item_text(ID_PROXY_TRACKERS, TR("proxy_tracker_connections"));

}

void connection_page::enable_window(int id, bool enabled)
{
    EnableWindow(
        GetDlgItem(handle(), id),
        enabled ? TRUE : FALSE);
}

void connection_page::check_changed(HWND hDlg, UINT uCtrlId, UINT uCommand)
{
    switch (uCtrlId)
    {
    case ID_LISTEN_INTERFACES:
    case ID_PROXY_FORCE:
    case ID_PROXY_HOST:
    case ID_PROXY_HOSTNAMES:
    case ID_PROXY_PASSWORD:
    case ID_PROXY_PEERS:
    case ID_PROXY_PORT:
    case ID_PROXY_TRACKERS:
    case ID_PROXY_TYPE:
    case ID_PROXY_USERNAME:
    {
        switch (uCommand)
        {
        case BN_CLICKED:
        case CBN_SELENDOK:
        case EN_CHANGE:
        {
            PropSheet_Changed(GetParent(hDlg), hDlg);
            break;
        }
        }
        break;
    }
    }
}

bool connection_page::is_checked(int id)
{
    return IsDlgButtonChecked(handle(), id) == BST_CHECKED;
}
