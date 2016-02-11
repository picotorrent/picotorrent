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

        std::wstring get_listen_address();
        int get_listen_port();
        int get_proxy_type();
        std::wstring get_proxy_host();
        int get_proxy_port();
        std::wstring get_proxy_username();
        std::wstring get_proxy_password();
        bool get_proxy_force_checked();
        bool get_proxy_hostnames_checked();
        bool get_proxy_peers_checked();
        bool get_proxy_trackers_checked();

        void set_listen_address(const std::wstring &address);
        void set_listen_port(int port);
        void set_proxy_force_checked(bool enabled);
        void set_proxy_force_enabled(bool enabled);
        void set_proxy_hostnames_checked(bool enabled);
        void set_proxy_hostnames_enabled(bool enabled);
        void set_proxy_peers_checked(bool enabled);
        void set_proxy_peers_enabled(bool enabled);
        void set_proxy_trackers_checked(bool enabled);
        void set_proxy_trackers_enabled(bool enabled);
        void set_proxy_host(const std::wstring &value);
        void set_proxy_host_enabled(bool enabled);
        void set_proxy_port(const std::wstring &value);
        void set_proxy_port_enabled(bool enabled);
        void set_proxy_username(const std::wstring &value);
        void set_proxy_username_enabled(bool enabled);
        void set_proxy_password(const std::wstring &value);
        void set_proxy_password_enabled(bool enabled);
        void set_proxy_type(int type);
        void set_proxy_type_changed_callback(const std::function<void(int)> &callback);

    protected:
        std::vector<BYTE> get_address_bytes(const std::wstring &address);
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();
        
    private:
        void enable_window(int id, bool enabled);
        void check_changed(HWND hDlg, UINT uCtrlId, UINT uCommand);
        bool is_checked(int id);
        std::wstring get_window_text(int id);

        std::function<void(int)> proxy_type_changed_cb_;
    };
}
}
}
}
