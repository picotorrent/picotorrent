#include "mainwindow.h"

#include <libtorrent/alert_types.hpp>

LRESULT CMainWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 0;
}

LRESULT CMainWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = FALSE;
    return 1;
}

LRESULT CMainWindow::OnSessionAlert(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    libtorrent::alert* alert = (libtorrent::alert*)lParam;

    switch (alert->type())
    {
    default:
        break;
    }

    return 0;
}
