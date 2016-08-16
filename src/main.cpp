#include "stdafx.h"

#include "CMainFrame.hpp"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    CMainFrame wndMain;

    if (wndMain.Create() == NULL)
    {
        ATLTRACE(_T("Main window creation failed!\n"));
        return 0;
    }

    wndMain.ShowWindow(nCmdShow);

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
    ::InitCommonControls();
    ::SetProcessDPIAware();

    _Module.Init(NULL, hInstance);

    int nRet = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();
    return nRet;
}
