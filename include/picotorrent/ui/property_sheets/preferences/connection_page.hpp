#pragma once

#include <picotorrent/ui/property_sheets/property_sheet_page.hpp>

#include <functional>
#include <string>
#include <vector>
#include <windows.h>

namespace picotorrent
{
namespace ui
{
namespace property_sheets
{
namespace preferences
{
    class connection_page : public property_sheet_page
    {
    public:
        connection_page();

        void add_proxy_type(const std::wstring &name, int type);
        void set_listen_address(const std::wstring &address);
        void set_listen_port(int port);

        void set_proxy_host_enabled(bool enabled);
        void set_proxy_port_enabled(bool enabled);
        void set_proxy_type(int type);
        void set_proxy_type_changed_callback(const std::function<void(int)> &callback);

    protected:
        std::vector<BYTE> get_address_bytes(const std::wstring &address);
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        
    private:
        std::function<void(int)> proxy_type_changed_cb_;
    };
}
}
}
}
