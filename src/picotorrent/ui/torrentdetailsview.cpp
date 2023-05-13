#include "torrentdetailsview.hpp"

#include <wx/notebook.h>
#include <wx/sizer.h>

#include "../core/configuration.hpp"
#include "torrentdetailsfilespanel.hpp"
#include "torrentdetailsoverviewpanel.hpp"
#include "torrentdetailspeerspanel.hpp"
#include "torrentdetailstrackerspanel.hpp"
#include "translator.hpp"

using pt::UI::TorrentDetailsView;

TorrentDetailsView::TorrentDetailsView(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Core::Configuration> cfg)
    : wxNotebook(parent, id),
    m_cfg(cfg),
    m_overview(new TorrentDetailsOverviewPanel(this, wxID_ANY, cfg->IsDarkMode())),
    m_files(new TorrentDetailsFilesPanel(this, wxID_ANY)),
    m_peers(new TorrentDetailsPeersPanel(this, wxID_ANY)),
    m_trackers(new TorrentDetailsTrackersPanel(this, wxID_ANY))
{
    this->AddPage(m_overview, i18n("overview"));
    this->AddPage(m_files,    i18n("files"));
    this->AddPage(m_peers,    i18n("peers"));
    this->AddPage(m_trackers, i18n("trackers"));
    this->ReloadConfiguration();
}

TorrentDetailsView::~TorrentDetailsView()
{
}

void TorrentDetailsView::Refresh(std::map<lt::info_hash_t, pt::BitTorrent::TorrentHandle*> const& torrents)
{
    if (torrents.size() != 1)
    {
        this->Reset();
        return;
    }

    m_overview->Refresh(torrents.begin()->second);
    m_files->Refresh(torrents.begin()->second);
    m_peers->Refresh(torrents.begin()->second);
    m_trackers->Refresh(torrents.begin()->second);
}

void TorrentDetailsView::ReloadConfiguration()
{
    auto showPieceProgress = m_cfg->Get<bool>("ui.torrent_overview.show_piece_progress");
    auto cols = m_cfg->Get<int>("ui.torrent_overview.columns");

    m_overview->UpdateView(
        cols.value_or(2),
        showPieceProgress.value_or(true));
}

void TorrentDetailsView::Reset()
{
    m_overview->Reset();
    m_files->Reset();
    m_peers->Reset();
    m_trackers->Reset();
}
