#pragma once

#include <memory>
#include <string>

#include <atlbase.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atldlgs.h>

#include "../resources.h"

namespace Wizard
{
    struct WizardState;

    class ConfigurePage : public CPropertyPageImpl<ConfigurePage>
    {
        friend class CPropertyPageImpl<ConfigurePage>;

    public:
        enum { IDD = IDD_WIZARD_CONFIGURE };

        ConfigurePage(std::shared_ptr<WizardState> state);
		~ConfigurePage();

    private:
        LRESULT OnInitDialog(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
        BOOL OnSetActive();

        BEGIN_MSG_MAP(ConfigurePage)
            MESSAGE_HANDLER_EX(WM_INITDIALOG, OnInitDialog)
            CHAIN_MSG_MAP(__super)
        END_MSG_MAP()

        std::wstring m_title;
        std::shared_ptr<WizardState> m_state;
    };
}
