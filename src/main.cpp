#include "stdafx.h"

#include "CMainFrame.hpp"
#include "CommandLine.hpp"
#include "UnhandledExceptionHandler.hpp"

#include <signal.h>

CAppModule _Module;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    CMainFrame wndMain;

    if (!wndMain.IsSingleInstance())
    {
        wndMain.ActivateOtherInstance(lpstrCmdLine);
        return 0;
    }

    if (wndMain.CreateEx() == NULL)
    {
        ATLTRACE(_T("Main window creation failed!\n"));
        return 0;
    }

    wndMain.Show(nCmdShow);

    // Handle command line
    CommandLine cmd = CommandLine::Parse(lpstrCmdLine);
    wndMain.HandleCommandLine(cmd);

    int nRet = theLoop.Run();

    _Module.RemoveMessageLoop();
    return nRet;
}

int WINAPI wWinMain(
    _In_ HINSTANCE     hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR        lpstrCmdLine,
    _In_ int           nCmdShow)
{
    UnhandledExceptionHandler::Setup();

    ::CoInitialize(NULL);
    ::InitCommonControls();

    INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX) };
    icc.dwICC = ICC_BAR_CLASSES;
    ::InitCommonControlsEx(&icc);

    ::SetProcessDPIAware();

    _Module.Init(NULL, hInstance);

    int nRet = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();
    ::CoUninitialize();

    return nRet;
}
