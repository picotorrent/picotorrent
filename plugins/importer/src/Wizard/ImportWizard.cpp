#include "ImportWizard.hpp"

#include "ConfigurePage.hpp"
#include "PreviewPage.hpp"
#include "WelcomePage.hpp"

using Wizard::ImportWizard;

ImportWizard::ImportWizard()
    : CPropertySheetImpl<ImportWizard>(TEXT("Import torrents")),
    m_preview(std::make_unique<PreviewPage>()),
    m_configure(std::make_unique<ConfigurePage>()),
    m_welcome(std::make_unique<WelcomePage>())
{
    // Add pages
    AddPage(*m_welcome);
    AddPage(*m_configure);
    AddPage(*m_preview);

    m_psh.dwFlags |= PSH_AEROWIZARD;
}

ImportWizard::~ImportWizard()
{
}
