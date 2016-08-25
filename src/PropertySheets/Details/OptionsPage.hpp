#pragma once

#include "../../resources.h"
#include "../../stdafx.h"

#include <string>

namespace PropertySheets
{
namespace Details
{
    class OptionsPage : public CPropertyPageImpl<OptionsPage>
    {
        friend class CPropertyPageImpl<OptionsPage>;

    public:
        enum { IDD = IDD_DETAILS_OPTIONS };
        OptionsPage();

    protected:
        //BOOL OnApply();

    private:
        void OnDestroy();
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

        BEGIN_MSG_MAP_EX(OptionsPage)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)

            CHAIN_MSG_MAP(CPropertyPageImpl<OptionsPage>)
        END_MSG_MAP()

        std::wstring m_title;
    };
}
}
