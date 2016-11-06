#pragma once

#include "../resources.h"
#include "../stdafx.h"

namespace Dialogs
{
class AboutDialog : public CDialogImpl<AboutDialog>
{
public:
    enum { IDD = IDD_ABOUT };

    AboutDialog();
    ~AboutDialog();

private:
    LRESULT OnClick(LPNMHDR pnmh);
    void OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

    BEGIN_MSG_MAP_EX(AboutDialog)
        MSG_WM_INITDIALOG(OnInitDialog)
        NOTIFY_CODE_HANDLER_EX(NM_CLICK, OnClick)
        COMMAND_ID_HANDLER_EX(IDCANCEL, OnEndDialog)
        COMMAND_ID_HANDLER_EX(IDOK, OnEndDialog)
    END_MSG_MAP()

    HFONT m_font;
};
}
