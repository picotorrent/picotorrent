#pragma once

#include <string>

#include <atlbase.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atldlgs.h>

#include "../resources.h"

namespace Wizard
{
    class WelcomePage : public CPropertyPageImpl<WelcomePage>
    {
        friend class CPropertyPageImpl<WelcomePage>;

    public:
        enum { IDD = IDD_WIZARD_WELCOME };

        WelcomePage();

    private:
        LRESULT OnInitDialog(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
        void OnQBittorrentImport(UINT /*nofityCode*/, int /*commandId*/, HWND /*handle*/);
        BOOL OnSetActive();

        BEGIN_MSG_MAP(WelcomePage)
            MESSAGE_HANDLER_EX(WM_INITDIALOG, OnInitDialog)

            COMMAND_HANDLER_EX(ID_BTN_QBITTORRENT, BN_CLICKED, OnQBittorrentImport);

            CHAIN_MSG_MAP(__super)
        END_MSG_MAP()

        std::wstring m_title;
    };
}
