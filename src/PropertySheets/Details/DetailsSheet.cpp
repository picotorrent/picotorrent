#include "DetailsSheet.hpp"

#include "FilesPage.hpp"
#include "OptionsPage.hpp"
#include "OverviewPage.hpp"
#include "PeersPage.hpp"
#include "../../Translator.hpp"

using PropertySheets::Details::DetailsSheet;

DetailsSheet::DetailsSheet(const libtorrent::torrent_handle& th)
    : m_overview(std::make_unique<OverviewPage>()),
    m_options(std::make_unique<OptionsPage>()),
    m_files(std::make_unique<FilesPage>(th)),
    m_peers(std::make_unique<PeersPage>(th))
{
    AddPage(*m_overview);
    AddPage(*m_options);
    AddPage(*m_files);
    AddPage(*m_peers);
    //AddPage(*m_connection);*/
}

DetailsSheet::~DetailsSheet()
{
}

void DetailsSheet::OnSheetInitialized()
{
    SetTitle(TRW("preferences"));
}
