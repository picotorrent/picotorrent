#pragma once

#include "../../stdafx.h"

#include <memory>

namespace libtorrent
{
    struct torrent_handle;
}

namespace PropertySheets
{
namespace Details
{
    class FilesPage;
    class OptionsPage;
    class OverviewPage;
    class PeersPage;

    class DetailsSheet : public CPropertySheetImpl<DetailsSheet>
    {
        friend class CPropertySheetImpl<DetailsSheet>;

    public:
        DetailsSheet(const libtorrent::torrent_handle& th);
        ~DetailsSheet();

    protected:
        void OnSheetInitialized();

    private:
        std::unique_ptr<FilesPage> m_files;
        std::unique_ptr<OptionsPage> m_options;
        std::unique_ptr<OverviewPage> m_overview;
        std::unique_ptr<PeersPage> m_peers;
    };
}
}
