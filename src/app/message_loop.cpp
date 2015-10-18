#include <picotorrent/app/message_loop.hpp>

#include <windows.h>

using picotorrent::app::message_loop;

int message_loop::run()
{
    MSG msg;
    BOOL ret;

    while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (ret == -1)
        {
            // TODO(handle error)
        }
        else
        {
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}
