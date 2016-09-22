#pragma once

#include <string>

#include <atlbase.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atldlgs.h>

#include "../resources.h"

namespace Wizard
{
    class PreviewPage : public CPropertyPageImpl<PreviewPage>
    {
        friend class CPropertyPageImpl<PreviewPage>;

    public:
        enum { IDD = IDD_WIZARD_PREVIEW };

        PreviewPage();

    private:
        BOOL OnSetActive();

        BEGIN_MSG_MAP(PreviewPage)
            CHAIN_MSG_MAP(__super)
        END_MSG_MAP()

        std::wstring m_title;
    };
}
