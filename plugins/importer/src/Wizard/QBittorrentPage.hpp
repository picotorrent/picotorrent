#pragma once

#include <string>

#include <atlbase.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atldlgs.h>

#include "../resources.h"

namespace Wizard
{
    class QBittorrentPage : public CPropertyPageImpl<QBittorrentPage>
    {
        friend class CPropertyPageImpl<QBittorrentPage>;

    public:
        enum { IDD = IDD_WIZARD_QBITTORRENT };

        QBittorrentPage();

    private:
        LRESULT OnInitDialog(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
        BOOL OnSetActive();
        int OnWizardNext();

        BEGIN_MSG_MAP(QBittorrentPage)
            MESSAGE_HANDLER_EX(WM_INITDIALOG, OnInitDialog)
            CHAIN_MSG_MAP(__super)
        END_MSG_MAP()

        std::wstring m_title;
    };
}
