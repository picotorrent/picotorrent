#pragma once

#include <memory>

#include <atlbase.h>
#include <atlapp.h>
#include <atldlgs.h>

namespace Wizard
{
    class PreviewPage;
    class QBittorrentPage;
    class UTorrentPage;
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

        std::unique_ptr<PreviewPage> m_preview;
        std::unique_ptr<QBittorrentPage> m_qbittorrent;
        std::unique_ptr<UTorrentPage> m_utorrent;
        std::unique_ptr<WelcomePage> m_welcome;
    };
}
