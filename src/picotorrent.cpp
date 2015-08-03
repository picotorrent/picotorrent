#include "picotorrent.h"

CPicoTorrent::CPicoTorrent(HINSTANCE hInstance)
{
    module_.Init(NULL, hInstance);
    module_.AddMessageLoop(&loop_);

    // Set up main application window
    window_.CreateEx();
    window_.ResizeClient(800, 400);
}

CPicoTorrent::~CPicoTorrent()
{
    module_.RemoveMessageLoop();
    module_.Term();
}

int CPicoTorrent::Run(LPWSTR lpCmdLine, int nCmdShow)
{
    window_.ShowWindow(nCmdShow);
    return loop_.Run();
}
