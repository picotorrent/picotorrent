#include "logging.h"
#include "picotorrent.h"

int WINAPI wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE,
    _In_     LPWSTR,
    _In_     int       nCmdShow)
{
    pico::Logging::Init();

    pico::PicoTorrent pt(hInstance);

    if (!pt.Init())
    {
        return 0;
    }

    return pt.Run(nCmdShow);
}
