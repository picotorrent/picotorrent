#ifndef _PT_UI_ABOUTDIALOG_H
#define _PT_UI_ABOUTDIALOG_H

#include "../stdafx.h"

class CAboutDialog
    : public CDialogImpl<CAboutDialog>
{
public:
    enum { IDD = IDD_ABOUTDIALOG };

    BEGIN_MSG_MAP(CAboutDialog)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    END_MSG_MAP()

protected:
    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CenterWindow(GetParent());
        return TRUE;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return FALSE;
    }
};

#endif
