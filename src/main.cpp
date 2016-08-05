// Enable visual styles
#pragma comment(linker, "\"/manifestdependency:type='win32' \
                        name='Microsoft.Windows.Common-Controls' \
                        version='6.0.0.0' \
                        processorArchitecture='*' \
                        publicKeyToken='6595b64144ccf1df' \
                        language='*'\"")

#include <windows.h>

#include <picotorrent/client/application.hpp>
#include <picotorrent/common/command_line.hpp>
#include <picotorrent/server/application.hpp>

using picotorrent::common::command_line;

int WINAPI wWinMain(
    _In_ HINSTANCE     hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR        lpCmdLine,
    _In_ int           nCmdShow)
{
    command_line cmd = command_line::parse(lpCmdLine);
    std::shared_ptr<picotorrent::common::application> app;

    if (cmd.daemon())
    {
        auto server_app = std::make_shared<picotorrent::server::application>();

        if (cmd.alloc_console())
        {
            server_app->allocate_console();
        }

        app = server_app;
    }
    else
    {
        picotorrent::client::application::wait_for_restart(cmd);
        app = std::make_shared<picotorrent::client::application>();
    }

    if (!app->init())
    {
        return 1;
    }

    return app->run(cmd);
}
