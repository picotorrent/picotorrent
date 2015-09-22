#include "application.h"
#include "stdafx.h"

int WINAPI WinMain(
    _In_ HINSTANCE     hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR         lpCmdLine,
    _In_ int           nCmdShow)
{
    pico::Application app;

    if(!app.Initialize())
    {
        return 0;
    }

    return (int)app.Run();
}
