// Enable visual styles
#pragma comment(linker, "\"/manifestdependency:type='win32' \
                        name='Microsoft.Windows.Common-Controls' \
                        version='6.0.0.0' \
                        processorArchitecture='*' \
                        publicKeyToken='6595b64144ccf1df' \
                        language='*'\"")

#include <windows.h>

#include <picotorrent/app/application.hpp>

int WINAPI WinMain(
    _In_ HINSTANCE     hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR         lpCmdLine,
    _In_ int           nCmdShow)
{
    picotorrent::app::application app;

    if (!app.init())
    {
        return 0;
    }

    return app.run();
}
