#include "NotifyIconController.hpp"

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>

#include "../Controllers/AddTorrentController.hpp"
#include "../Dialogs/AddMagnetLinkDialog.hpp"
#include "../resources.h"
#include "../stdafx.h"
#include "../Translator.hpp"

namespace lt = libtorrent;
using Controllers::NotifyIconController;

NotifyIconController::NotifyIconController(HWND hWnd, const std::shared_ptr<lt::session>& session)
    : m_hWnd(hWnd),
    m_session(session)
{
}

NotifyIconController::~NotifyIconController()
{
}

void NotifyIconController::Execute(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(lParam))
    {
    case WM_CONTEXTMENU:
    {
        POINT p{ GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam) };

        HMENU menu = CreatePopupMenu();
        AppendMenu(menu, MF_STRING, ID_NOTIFYICON_ADD, TRW("add_torrent"));
        AppendMenu(menu, MF_STRING, ID_NOTIFYICON_ADD_MAGNET, TRW("add_magnet_link_s"));
        AppendMenu(menu, MF_SEPARATOR, 0, NULL);
        AppendMenu(menu, MF_STRING, ID_NOTIFYICON_EXIT, TRW("exit"));

        SetForegroundWindow(m_hWnd);
        int res = TrackPopupMenuEx(menu, TPM_NONOTIFY | TPM_RETURNCMD, p.x, p.y, m_hWnd, NULL);

        DestroyMenu(menu);

        switch (res)
        {
        case ID_NOTIFYICON_ADD:
        {
            Controllers::AddTorrentController ctrl(m_session);
            ctrl.Execute();
            break;
        }
        case ID_NOTIFYICON_ADD_MAGNET:
        {
            Dialogs::AddMagnetLinkDialog dlg;
            if (dlg.DoModal(m_hWnd) == IDOK)
            {
                std::vector<lt::torrent_info> ti = dlg.GetTorrentFiles();

                Controllers::AddTorrentController atc(m_session);
                atc.Execute(ti);
            }
            break;
        }
        case ID_NOTIFYICON_EXIT:
        {
            DestroyWindow(m_hWnd);
            break;
        }
        }

        break;
    }
    case WM_LBUTTONDBLCLK:
    {
        ShowWindow(m_hWnd, IsIconic(m_hWnd) ? SW_RESTORE : SW_SHOW);
        SetForegroundWindow(m_hWnd);
        break;
    }
    }
}
