#include <windows.h>
#include "picotorrent.h"

int WINAPI wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPWSTR    lpCmdLine,
    _In_     int       nCmdShow)
{
    CPicoTorrent pico(hInstance);
    return pico.Run(lpCmdLine, nCmdShow);
}
