#include "ImportWizard.hpp"

#include <picotorrent/api.hpp>

#include "ConfigurePage.hpp"
#include "PreviewPage.hpp"
#include "WelcomePage.hpp"
#include "WizardState.hpp"

using Wizard::ImportWizard;

ImportWizard::ImportWizard(std::shared_ptr<IPicoTorrent> pico)
    : CPropertySheetImpl<ImportWizard>(TEXT("Import torrents")),
    m_state(std::make_shared<WizardState>()),
    m_preview(std::make_unique<PreviewPage>(m_state)),
    m_configure(std::make_unique<ConfigurePage>(m_state)),
    m_welcome(std::make_unique<WelcomePage>(pico, m_state))
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
