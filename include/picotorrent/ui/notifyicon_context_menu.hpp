#pragma once

#include <windows.h>

namespace picotorrent
{
namespace ui
{
    class notifyicon_context_menu
    {
    public:
        notifyicon_context_menu();
        ~notifyicon_context_menu();

        int show(HWND hWnd, const POINT &p);

    private:
        HMENU menu_;
    };
}
}
