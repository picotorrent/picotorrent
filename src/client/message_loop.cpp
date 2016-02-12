#include <picotorrent/client/message_loop.hpp>

#include <windows.h>

using picotorrent::client::message_loop;

int message_loop::run(HWND handle, HACCEL accel)
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
            if (TranslateAccelerator(handle, accel, &msg) == 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    return (int)msg.wParam;
}
