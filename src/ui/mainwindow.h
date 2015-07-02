#include <atlbase.h>
#include <atlapp.h>
#include <atlframe.h>
#include <windows.h>

#include "../resources.h"

class CMainWindow
    : public CFrameWindowImpl<CMainWindow>
{
public:
    DECLARE_FRAME_WND_CLASS_EX(NULL, IDR_MAINWINDOW, 0, -1);

    BEGIN_MSG_MAP(CMainWindow)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainWindow>)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_APP + 0x01, OnSessionAlert)
    END_MSG_MAP()

protected:
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

    LRESULT OnSessionAlert(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
