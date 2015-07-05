#ifndef _PT_PICOTORRENT_H
#define _PT_PICOTORRENT_H

#include <atlbase.h>
#include <atlapp.h>
#include <windows.h>

#include "ui/mainwindow.h"

class CPicoTorrent
{
public:
    CPicoTorrent(HINSTANCE hInstance);
    ~CPicoTorrent();

    int Run(LPWSTR lpCmdLine, int nCmdShow = SW_SHOWDEFAULT);

private:
    CAppModule module_;
    CMessageLoop loop_;
    CMainWindow window_;
};

#endif
