#include <windows.h>
#include "picotorrent.h"

#pragma comment(linker, "\"/manifestdependency:type='win32' \
                            name='Microsoft.Windows.Common-Controls' \
                            version='6.0.0.0' \
                            processorArchitecture='*' \
                            publicKeyToken='6595b64144ccf1df' \
                            language='*'\"")

#ifdef _DEBUG
#pragma comment(lib, "libboost_random-vc120-mt-gd-1_58.lib")
#else
#pragma comment(lib, "libboost_random-vc120-mt-1_58.lib")
#endif

int WINAPI wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPWSTR    lpCmdLine,
    _In_     int       nCmdShow)
{
    CPicoTorrent pico(hInstance);
    return pico.Run(lpCmdLine, nCmdShow);
}
