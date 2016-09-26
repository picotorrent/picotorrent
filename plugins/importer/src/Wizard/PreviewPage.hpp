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

    class PreviewPage : public CPropertyPageImpl<PreviewPage>
    {
        friend class CPropertyPageImpl<PreviewPage>;

    public:
        enum { IDD = IDD_WIZARD_PREVIEW };

        PreviewPage(std::shared_ptr<WizardState> state);

    private:
        LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam);
        BOOL OnSetActive();

        BEGIN_MSG_MAP(PreviewPage)
            MESSAGE_HANDLER_EX(WM_INITDIALOG, OnInitDialog)
            CHAIN_MSG_MAP(__super)
        END_MSG_MAP()

        std::wstring m_title;
        std::shared_ptr<WizardState> m_state;
    };
}
