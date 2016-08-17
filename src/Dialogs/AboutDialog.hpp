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
    void OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

    BEGIN_MSG_MAP_EX(AboutDialog)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_ID_HANDLER_EX(IDCANCEL, OnEndDialog)
        COMMAND_ID_HANDLER_EX(IDOK, OnEndDialog)
    END_MSG_MAP()

    HFONT m_font;
};
}
