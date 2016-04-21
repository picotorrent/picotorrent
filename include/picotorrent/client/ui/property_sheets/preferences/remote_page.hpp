#pragma once

#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>
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
    class remote_page : public property_sheet_page
    {
    public:
        remote_page();
        ~remote_page();

        bool enable_websocket_api();
        void set_certificate_public_key(const std::string &pubkey);
        void set_enable_websocket_api(bool value);
        void set_websocket_access_token(const std::string &token);
        void set_websocket_port(int port);
        int websocket_port();

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();

    private:
        void check_changed(HWND hDlg, UINT uCtrlId, UINT uCommand);

        HFONT edit_font_;
    };
}
}
}
}
}
