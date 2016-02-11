// Enable visual styles
#pragma comment(linker, "\"/manifestdependency:type='win32' \
                        name='Microsoft.Windows.Common-Controls' \
                        version='6.0.0.0' \
                        processorArchitecture='*' \
                        publicKeyToken='6595b64144ccf1df' \
                        language='*'\"")

#include <windows.h>

#include <picotorrent/app/application.hpp>

int WINAPI wWinMain(
    _In_ HINSTANCE     hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR        lpCmdLine,
    _In_ int           nCmdShow)
{
    picotorrent::app::application::wait_for_restart(lpCmdLine);
    picotorrent::app::application app;

    if (!app.is_single_instance())
    {
        app.activate_other_instance(lpCmdLine);
        return -33;
    }

    if (!app.init())
    {
        return 0;
    }

    return app.run(lpCmdLine);
}
