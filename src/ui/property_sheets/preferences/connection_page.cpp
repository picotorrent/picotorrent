#include <picotorrent/ui/property_sheets/preferences/connection_page.hpp>

#include <picotorrent/ui/resources.hpp>

#include <windowsx.h>
#include <commctrl.h>
#include <iostream>
#include <sstream>

using picotorrent::ui::property_sheets::preferences::connection_page;

connection_page::connection_page()
{
    set_flags(PSP_USETITLE);
    set_instance(GetModuleHandle(NULL));
    set_template_id(6768);
    set_title_id(IDS_PREFS_CONNECTION_TITLE);
}

void connection_page::add_proxy_type(const std::wstring &name, int type)
{
    HWND ctl = GetDlgItem(handle(), ID_PREFS_PROXY_TYPE);

    int index = ComboBox_AddString(ctl, name.c_str());
    ComboBox_SetItemData(ctl, index, type);
}

int connection_page::get_proxy_type()
{
    HWND ctl = GetDlgItem(handle(), ID_PREFS_PROXY_TYPE);
    int idx = ComboBox_GetCurSel(ctl);

    return (int)ComboBox_GetItemData(ctl, idx);
}

std::wstring connection_page::get_proxy_host()
{
    return get_window_text(ID_PREFS_PROXY_HOST);
}

std::wstring connection_page::get_proxy_port()
{
    return get_window_text(ID_PREFS_PROXY_PORT);
}

std::wstring connection_page::get_proxy_username()
{
    return get_window_text(ID_PREFS_PROXY_USERNAME);
}

std::wstring connection_page::get_proxy_password()
{
    return get_window_text(ID_PREFS_PROXY_PASSWORD);
}

bool connection_page::get_proxy_force_checked()
{
    return is_checked(ID_PREFS_PROXY_FORCE);
}

bool connection_page::get_proxy_hostnames_checked()
{
    return is_checked(ID_PREFS_PROXY_HOSTNAMES);
}

bool connection_page::get_proxy_peers_checked()
{
    return is_checked(ID_PREFS_PROXY_PEERS);
}

bool connection_page::get_proxy_trackers_checked()
{
    return is_checked(ID_PREFS_PROXY_TRACKERS);
}

void connection_page::set_listen_address(const std::wstring &address)
{
    std::vector<BYTE> addr = get_address_bytes(address);
    LPARAM l = MAKEIPADDRESS(addr[0], addr[1], addr[2], addr[3]);
    SendDlgItemMessage(handle(), 1010, IPM_SETADDRESS, 0, l);
}

void connection_page::set_listen_port(int port)
{
    SetDlgItemText(handle(), ID_PREFS_LISTENPORT, std::to_wstring(port).c_str());
}

void connection_page::set_proxy_force_enabled(bool enabled)
{
    enable_window(ID_PREFS_PROXY_FORCE, enabled);
}

void connection_page::set_proxy_hostnames_enabled(bool enabled)
{
    enable_window(ID_PREFS_PROXY_HOSTNAMES, enabled);
}

void connection_page::set_proxy_peers_enabled(bool enabled)
{
    enable_window(ID_PREFS_PROXY_PEERS, enabled);
}

void connection_page::set_proxy_trackers_enabled(bool enabled)
{
    enable_window(ID_PREFS_PROXY_TRACKERS, enabled);
}

void connection_page::set_proxy_host(const std::wstring &value)
{
    SetDlgItemText(handle(), ID_PREFS_PROXY_HOST, value.c_str());
}

void connection_page::set_proxy_port(const std::wstring &value)
{
    SetDlgItemText(handle(), ID_PREFS_PROXY_PORT, value.c_str());
}

void connection_page::set_proxy_username(const std::wstring &value)
{
    SetDlgItemText(handle(), ID_PREFS_PROXY_USERNAME, value.c_str());
}

void connection_page::set_proxy_password(const std::wstring &value)
{
    SetDlgItemText(handle(), ID_PREFS_PROXY_PASSWORD, value.c_str());
}

void connection_page::set_proxy_force_checked(bool checked)
{
    CheckDlgButton(handle(), ID_PREFS_PROXY_FORCE, checked ? BST_CHECKED : BST_UNCHECKED);
}

void connection_page::set_proxy_hostnames_checked(bool checked)
{
    CheckDlgButton(handle(), ID_PREFS_PROXY_HOSTNAMES, checked ? BST_CHECKED : BST_UNCHECKED);
}

void connection_page::set_proxy_peers_checked(bool checked)
{
    CheckDlgButton(handle(), ID_PREFS_PROXY_PEERS, checked ? BST_CHECKED : BST_UNCHECKED);
}

void connection_page::set_proxy_trackers_checked(bool checked)
{
    CheckDlgButton(handle(), ID_PREFS_PROXY_TRACKERS, checked ? BST_CHECKED : BST_UNCHECKED);
}

void connection_page::set_proxy_host_enabled(bool enabled)
{
    enable_window(ID_PREFS_PROXY_HOST, enabled);
}

void connection_page::set_proxy_port_enabled(bool enabled)
{
    enable_window(ID_PREFS_PROXY_PORT, enabled);
}

void connection_page::set_proxy_username_enabled(bool enabled)
{
    enable_window(ID_PREFS_PROXY_USERNAME, enabled);
}

void connection_page::set_proxy_password_enabled(bool enabled)
{
    enable_window(ID_PREFS_PROXY_PASSWORD, enabled);
}

void connection_page::set_proxy_type(int type)
{
    HWND ctl = GetDlgItem(handle(), ID_PREFS_PROXY_TYPE);

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
    case ID_PREFS_PROXY_TYPE:
    {
        if (HIWORD(wParam) == CBN_SELENDOK && proxy_type_changed_cb_)
        {
            HWND ctl = GetDlgItem(handle(), ID_PREFS_PROXY_TYPE);
            int index = ComboBox_GetCurSel(ctl);
            int type = (int)ComboBox_GetItemData(ctl, index);

            proxy_type_changed_cb_(type);
        }
        break;
    }

    case ID_PREFS_PROXY_FORCE:
    case ID_PREFS_PROXY_HOSTNAMES:
    case ID_PREFS_PROXY_PEERS:
    case ID_PREFS_PROXY_TRACKERS:
    {
        bool checked = IsDlgButtonChecked(hDlg, uCtrlId) == BST_CHECKED;
        CheckDlgButton(hDlg, uCtrlId, checked ? BST_UNCHECKED : BST_CHECKED);
        break;
    }
    }
    return FALSE;
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
    case 1010:
    case ID_PREFS_LISTENPORT:
    case ID_PREFS_PROXY_FORCE:
    case ID_PREFS_PROXY_HOST:
    case ID_PREFS_PROXY_HOSTNAMES:
    case ID_PREFS_PROXY_PASSWORD:
    case ID_PREFS_PROXY_PEERS:
    case ID_PREFS_PROXY_PORT:
    case ID_PREFS_PROXY_TRACKERS:
    case ID_PREFS_PROXY_TYPE:
    case ID_PREFS_PROXY_USERNAME:
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
