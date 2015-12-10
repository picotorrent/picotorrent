#pragma once

#include <windows.h>

namespace picotorrent
{
namespace app
{
    class message_loop
    {
    public:
        static int run(HWND handle, HACCEL accel);
    };
}
}
