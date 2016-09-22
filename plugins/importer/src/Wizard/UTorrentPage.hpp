#pragma once

#include <string>

#include <atlbase.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atldlgs.h>

#include "../resources.h"

namespace Wizard
{
    class UTorrentPage : public CPropertyPageImpl<UTorrentPage>
    {
        friend class CPropertyPageImpl<UTorrentPage>;

    public:
        enum { IDD = IDD_WIZARD_UTORRENT };

        UTorrentPage();

    private:
        LRESULT OnInitDialog(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
        BOOL OnSetActive();

        BEGIN_MSG_MAP(UTorrentPage)
            MESSAGE_HANDLER_EX(WM_INITDIALOG, OnInitDialog)
            CHAIN_MSG_MAP(__super)
        END_MSG_MAP()

        std::wstring m_title;
    };
}
