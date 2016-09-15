#pragma once

#include <string>

#include "../resources.h"
#include "../stdafx.h"

namespace Dialogs
{
class AddTrackerDialog : public CDialogImpl<AddTrackerDialog>
{
public:
    enum { IDD = IDD_ADD_TRACKER };

    AddTrackerDialog();
    ~AddTrackerDialog();

    std::wstring GetUrl();

private:
    void OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

    BEGIN_MSG_MAP_EX(AddTrackerDialog)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_ID_HANDLER_EX(IDOK, OnEndDialog)
    END_MSG_MAP()

    std::wstring m_url;
};
}
