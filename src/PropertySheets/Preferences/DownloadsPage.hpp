#pragma once

#include "../../resources.h"
#include "../../stdafx.h"

#include <string>

namespace PropertySheets
{
namespace Preferences
{
    class DownloadsPage : public CPropertyPageImpl<DownloadsPage>
    {
        friend class CPropertyPageImpl<DownloadsPage>;

    public:
        enum { IDD = IDD_PREFERENCES_DOWNLOADS };
        DownloadsPage();

    protected:
        BOOL OnApply();

    private:
        void OnBrowse(UINT uNotifyCode, int nID, CWindow wndCtl);
        void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
        void UpdateMoveState(bool checked);

        BEGIN_MSG_MAP_EX(DownloadsPage)
            MSG_WM_INITDIALOG(OnInitDialog)
            COMMAND_ID_HANDLER_EX(ID_PREFS_DEFSAVEPATH_BROWSE, OnBrowse)
            COMMAND_ID_HANDLER_EX(ID_PREFS_MOVEPATH_BROWSE, OnBrowse)
            MSG_WM_COMMAND(OnCommand)
            CHAIN_MSG_MAP(CPropertyPageImpl<DownloadsPage>)
        END_MSG_MAP()

        std::wstring m_title;

        CEdit m_savePath;
        CButton m_moveCompleted;
        CEdit m_movePath;
        CButton m_movePathBrowse;
        CButton m_onlyMoveFromDefault;
        CEdit m_dl;
        CEdit m_ul;
    };
}
}
