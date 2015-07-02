#include <windows.h>

#include <atlbase.h>
#include <atlapp.h>
#include <memory>

#include <libtorrent/session.hpp>
#include "ui/mainwindow.h"

class CPicoTorrent
{
public:
    CPicoTorrent(HINSTANCE hInstance);
    ~CPicoTorrent();

    int Run(LPWSTR lpCmdLine, int nCmdShow = SW_SHOWDEFAULT);

protected:
    void OnSessionAlert(std::auto_ptr<libtorrent::alert> alert);

private:
    CAppModule module_;
    CMessageLoop loop_;
    CMainWindow window_;

    std::unique_ptr<libtorrent::session> session_;
};
