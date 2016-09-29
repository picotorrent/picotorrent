#pragma once

#include <memory>
#include <string>

#include <atlbase.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atldlgs.h>

#include "../resources.h"

class IPicoTorrent;

namespace Wizard
{
    struct WizardState;

    class WelcomePage : public CPropertyPageImpl<WelcomePage>
    {
        friend class CPropertyPageImpl<WelcomePage>;

    public:
        enum { IDD = IDD_WIZARD_WELCOME };

        WelcomePage(std::shared_ptr<IPicoTorrent> pico, std::shared_ptr<WizardState> state);

    private:
        void OnQBittorrentImport(UINT /*nofityCode*/, int /*commandId*/, HWND /*handle*/);
        void OnUTorrentImport(UINT /*nofityCode*/, int /*commandId*/, HWND /*handle*/);
        BOOL OnSetActive();

        BEGIN_MSG_MAP(WelcomePage)
            COMMAND_HANDLER_EX(ID_BTN_QBITTORRENT, BN_CLICKED, OnQBittorrentImport)
            COMMAND_HANDLER_EX(ID_BTN_UTORRENT, BN_CLICKED, OnUTorrentImport)
            CHAIN_MSG_MAP(__super)
        END_MSG_MAP()

        std::wstring m_title;
        std::shared_ptr<IPicoTorrent> m_pico;
        std::shared_ptr<WizardState> m_state;
    };
}
