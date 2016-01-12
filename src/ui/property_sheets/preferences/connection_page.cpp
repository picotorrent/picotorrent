#include <picotorrent/ui/property_sheets/preferences/connection_page.hpp>

#include <picotorrent/ui/resources.hpp>

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
