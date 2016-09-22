#include "ImportWizard.hpp"

#include "PreviewPage.hpp"
#include "QBittorrentPage.hpp"
#include "UTorrentPage.hpp"
#include "WelcomePage.hpp"

using Wizard::ImportWizard;

ImportWizard::ImportWizard()
    : CPropertySheetImpl<ImportWizard>(TEXT("Import torrents")),
    m_preview(std::make_unique<PreviewPage>()),
    m_qbittorrent(std::make_unique<QBittorrentPage>()),
    m_utorrent(std::make_unique<UTorrentPage>()),
    m_welcome(std::make_unique<WelcomePage>())
{
    // Add pages
    AddPage(*m_welcome);
    AddPage(*m_qbittorrent);
    AddPage(*m_utorrent);
    AddPage(*m_preview);

    m_psh.dwFlags |= PSH_AEROWIZARD;
}

ImportWizard::~ImportWizard()
{
}
