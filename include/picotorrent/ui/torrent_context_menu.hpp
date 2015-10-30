#pragma once

#include <windows.h>

namespace picotorrent
{
namespace ui
{
    class torrent_context_menu
    {
    public:
        explicit torrent_context_menu();

        void highlight_pause();
        void highlight_resume();
        void remove_pause();
        void remove_resume();

        int show(HWND hWnd, const POINT &p);

    private:
        HMENU menu_;
    };
}
}
