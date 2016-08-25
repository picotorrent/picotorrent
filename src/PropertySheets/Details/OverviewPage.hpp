#pragma once

#include "../../resources.h"
#include "../../stdafx.h"

#include <string>

namespace PropertySheets
{
namespace Details
{
    class OverviewPage : public CPropertyPageImpl<OverviewPage>
    {
        friend class CPropertyPageImpl<OverviewPage>;

    public:
        enum { IDD = IDD_DETAILS_OVERVIEW };
        OverviewPage();

    protected:
        //BOOL OnApply();

    private:
        void OnDestroy();
        LRESULT OnFoo(UINT uMsg, WPARAM wParam, LPARAM lParam);
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

        /*void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
        void SelectComboBoxItemWithData(CComboBox& cb, int data);*/

        BEGIN_MSG_MAP_EX(OverviewPage)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)

            MESSAGE_HANDLER_EX(8989, OnFoo)
            CHAIN_MSG_MAP(CPropertyPageImpl<OverviewPage>)
        END_MSG_MAP()

        std::wstring m_title;
    };
}
}
