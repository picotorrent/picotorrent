#include <picotorrent/client/ui/property_sheets/preferences/connection_page.hpp>

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

void connection_page::add_proxy_type(const std::wstring &name, int type)
{
    HWND ctl = GetDlgItem(handle(), ID_PROXY_TYPE);

    int index = ComboBox_AddString(ctl, name.c_str());
    ComboBox_SetItemData(ctl, index, type);
}

std::wstring connection_page::get_listen_address()
{
    HWND ctl = GetDlgItem(handle(), ID_IPADDRESS);
    DWORD dwAddr = 0;

    SendMessage(ctl, IPM_GETADDRESS, 0, (LPARAM)&dwAddr);
    wchar_t wchAddr[20];
    StringCchPrintf(wchAddr, _countof(wchAddr), L"%ld.%ld.%ld.%ld",
        FIRST_IPADDRESS(dwAddr),
        SECOND_IPADDRESS(dwAddr),
        THIRD_IPADDRESS(dwAddr),
        FOURTH_IPADDRESS(dwAddr));

    return wchAddr;
}

int connection_page::get_listen_port()
{
    std::wstring text = get_window_text(ID_LISTENPORT);
    if (text.empty()) { return -1; }
    return std::stoi(text);
}

int connection_page::get_proxy_type()
{
    HWND ctl = GetDlgItem(handle(), ID_PROXY_TYPE);
    int idx = ComboBox_GetCurSel(ctl);

    return (int)ComboBox_GetItemData(ctl, idx);
}

std::wstring connection_page::get_proxy_host()
{
    return get_window_text(ID_PROXY_HOST);
}

int connection_page::get_proxy_port()
{
    std::wstring text = get_window_text(ID_PROXY_PORT);
    if (text.empty()) { return -1; }
    return std::stoi(text);
}

std::wstring connection_page::get_proxy_username()
{
    return get_window_text(ID_PROXY_USERNAME);
}

std::wstring connection_page::get_proxy_password()
{
    return get_window_text(ID_PROXY_PASSWORD);
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

void connection_page::set_listen_address(const std::wstring &address)
{
    std::vector<BYTE> addr = get_address_bytes(address);
    LPARAM l = MAKEIPADDRESS(addr[0], addr[1], addr[2], addr[3]);
    SendDlgItemMessage(handle(), ID_IPADDRESS, IPM_SETADDRESS, 0, l);
}

void connection_page::set_listen_port(int port)
{
    SetDlgItemText(handle(), ID_LISTENPORT, std::to_wstring(port).c_str());
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

void connection_page::set_proxy_host(const std::wstring &value)
{
    SetDlgItemText(handle(), ID_PROXY_HOST, value.c_str());
}

void connection_page::set_proxy_port(const std::wstring &value)
{
    SetDlgItemText(handle(), ID_PROXY_PORT, value.c_str());
}

void connection_page::set_proxy_username(const std::wstring &value)
{
    SetDlgItemText(handle(), ID_PROXY_USERNAME, value.c_str());
}

void connection_page::set_proxy_password(const std::wstring &value)
{
    SetDlgItemText(handle(), ID_PROXY_PASSWORD, value.c_str());
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

std::vector<BYTE> connection_page::get_address_bytes(const std::wstring &address)
{
    std::vector<BYTE> res;
    std::wistringstream f(address);
    std::wstring s;

    while (std::getline(f, s, L'.'))
    {
        res.push_back((BYTE)std::stoi(s));
    }

    return res;
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
    SetDlgItemText(handle(), ID_LISTEN_INTERFACE_GROUP, TR("listen_interface"));
    SetDlgItemText(handle(), ID_ADDRESS_TEXT, TR("address"));
    SetDlgItemText(handle(), ID_PORT_TEXT, TR("port"));
    SetDlgItemText(handle(), ID_PROXY_GROUP, TR("proxy"));
    SetDlgItemText(handle(), ID_TYPE_TEXT, TR("type"));
    SetDlgItemText(handle(), ID_HOST_TEXT, TR("host"));
    SetDlgItemText(handle(), ID_PROXY_PORT_TEXT, TR("port"));
    SetDlgItemText(handle(), ID_PROXY_USERNAME_TEXT, TR("username"));
    SetDlgItemText(handle(), ID_PROXY_PASSWORD_TEXT, TR("password"));
    SetDlgItemText(handle(), ID_PROXY_FORCE, TR("force_proxy"));
    SetDlgItemText(handle(), ID_PROXY_HOSTNAMES, TR("proxy_hostnames"));
    SetDlgItemText(handle(), ID_PROXY_PEERS, TR("proxy_peer_connections"));
    SetDlgItemText(handle(), ID_PROXY_TRACKERS, TR("proxy_tracker_connections"));

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
    case ID_IPADDRESS:
    case ID_LISTENPORT:
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

std::wstring connection_page::get_window_text(int id)
{
    TCHAR p[1024];
    GetDlgItemText(handle(), id, p, ARRAYSIZE(p));
    return p;
}
