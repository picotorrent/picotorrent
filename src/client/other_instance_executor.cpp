#include <picotorrent/client/other_instance_executor.hpp>

#include <windows.h>

#include <picotorrent/common/command_line.hpp>

using picotorrent::client::other_instance_executor;
using picotorrent::common::command_line;

int other_instance_executor::run(const command_line &cmd)
{
    HWND otherWindow = FindWindow(L"PicoTorrent/MainWindow", NULL);

    COPYDATASTRUCT cds;
    std::wstring args = cmd.raw();

    if (!args.empty())
    {
        cds.cbData = (DWORD)(sizeof(wchar_t) * (args.size() + 1));
        cds.dwData = 1;
        cds.lpData = (PVOID)&args[0];
    }

    // Activate other window
    SetForegroundWindow(otherWindow);
    ShowWindow(otherWindow, SW_RESTORE);

    SendMessage(otherWindow, WM_COPYDATA, NULL, (LPARAM)&cds);

    return 0;
}
