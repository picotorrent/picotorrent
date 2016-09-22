#pragma once

#include <memory>

#include <atlbase.h>
#include <atlapp.h>
#include <atldlgs.h>

namespace Wizard
{
	class ConfigurePage;
	class PreviewPage;
    class WelcomePage;

    class ImportWizard : public CPropertySheetImpl<ImportWizard>
    {
    public:
        ImportWizard();
        ~ImportWizard();

    private:
        BEGIN_MSG_MAP(ImportWizard)
            CHAIN_MSG_MAP(__super)
        END_MSG_MAP()

		std::unique_ptr<ConfigurePage> m_configure;
		std::unique_ptr<PreviewPage> m_preview;
        std::unique_ptr<WelcomePage> m_welcome;
    };
}
