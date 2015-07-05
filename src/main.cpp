#include <windows.h>
#include "picotorrent.h"

#pragma comment(linker, "\"/manifestdependency:type='win32' \
                            name='Microsoft.Windows.Common-Controls' \
                            version='6.0.0.0' \
                            processorArchitecture='*' \
                            publicKeyToken='6595b64144ccf1df' \
                            language='*'\"")

int WINAPI wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPWSTR    lpCmdLine,
    _In_     int       nCmdShow)
{
    CPicoTorrent pico(hInstance);
    return pico.Run(lpCmdLine, nCmdShow);
}
