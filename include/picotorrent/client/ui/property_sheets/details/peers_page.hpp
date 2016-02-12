#pragma once

#include <picotorrent/client/ui/property_sheets/property_sheet_page.hpp>
#include <map>
#include <string>
#include <vector>

#include <commctrl.h>
#include <uxtheme.h>

namespace picotorrent
{
namespace core
{
    class peer;
}
namespace client
{
namespace ui
{
namespace controls
{
    class list_view;
}
namespace property_sheets
{
namespace details
{
    class peers_page : public property_sheet_page
    {
    public:
        peers_page();
        ~peers_page();

        void refresh(const std::vector<core::peer> &peers);

    protected:
        BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam);
        void on_init_dialog();

    private:
        std::wstring on_list_display(const std::pair<int, int> &p);

        struct peer_state;
        std::unique_ptr<controls::list_view> list_;
        std::vector<peer_state> peers_;

    };
}
}
}
}
}
