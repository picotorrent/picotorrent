#include "DetailsSheet.hpp"

#include "FilesPage.hpp"
#include "OptionsPage.hpp"
#include "OverviewPage.hpp"
#include "PeersPage.hpp"
#include "TrackersPage.hpp"
#include "../../Translator.hpp"

using PropertySheets::Details::DetailsSheet;

DetailsSheet::DetailsSheet(const libtorrent::torrent_handle& th)
    : m_overview(std::make_unique<OverviewPage>(th)),
    m_options(std::make_unique<OptionsPage>(th)),
    m_files(std::make_unique<FilesPage>(th)),
    m_peers(std::make_unique<PeersPage>(th)),
    m_trackers(std::make_unique<TrackersPage>(th))
{
    AddPage(*m_overview);
    AddPage(*m_options);
    AddPage(*m_files);
    AddPage(*m_peers);
    AddPage(*m_trackers);
}

DetailsSheet::~DetailsSheet()
{
}

void DetailsSheet::OnSheetInitialized()
{
    SetTitle(TRW("preferences"));
}
