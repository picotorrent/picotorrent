#pragma once

#include <string>
#include <windows.h>

namespace picotorrent
{
namespace client
{
namespace ui
{
    class status_bar
    {
    public:
        status_bar(HWND hParent);
        HWND handle();
        void set_torrent_count(int count);
        void set_transfer_rates(int dl, int ul);

    private:
        std::wstring rate_to_string(int rate);
        HWND hWnd_;
    };
}
}
}
