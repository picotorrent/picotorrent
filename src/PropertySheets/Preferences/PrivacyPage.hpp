#pragma once

#include "../../resources.h"
#include "../../stdafx.h"

#include <string>

namespace PropertySheets
{
namespace Preferences
{
    class PrivacyPage : public CPropertyPageImpl<PrivacyPage>
    {
        friend class CPropertyPageImpl<PrivacyPage>;

    public:
        enum { IDD = IDD_PREFERENCES_PRIVACY };
        PrivacyPage();

    protected:
        BOOL OnApply();

    private:
        void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

        BEGIN_MSG_MAP_EX(PrivacyPage)
            MSG_WM_COMMAND(OnCommand)
            MSG_WM_INITDIALOG(OnInitDialog)
            CHAIN_MSG_MAP(CPropertyPageImpl<PrivacyPage>)
        END_MSG_MAP()

        std::wstring m_title;

        CEdit m_savePath;
        CEdit m_dl;
        CEdit m_ul;
    };
}
}
