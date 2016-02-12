#pragma once

#include <windows.h>

namespace picotorrent
{
namespace client
{
    class message_loop
    {
    public:
        static int run(HWND handle, HACCEL accel);
    };
}
}
