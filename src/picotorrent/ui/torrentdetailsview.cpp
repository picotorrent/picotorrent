#include "torrentdetailsview.hpp"

#include <wx/notebook.h>
#include <wx/sizer.h>

#include "torrentdetailsfilespanel.hpp"
#include "torrentdetailsoverviewpanel.hpp"
#include "torrentdetailspeerspanel.hpp"
#include "torrentdetailstrackerspanel.hpp"
#include "translator.hpp"

using pt::UI::TorrentDetailsView;

TorrentDetailsView::TorrentDetailsView(wxWindow* parent, wxWindowID id)
    : wxNotebook(parent, id),
    m_overview(new TorrentDetailsOverviewPanel(this, wxID_ANY)),
    m_files(new TorrentDetailsFilesPanel(this, wxID_ANY)),
    m_peers(new TorrentDetailsPeersPanel(this, wxID_ANY)),
    m_trackers(new TorrentDetailsTrackersPanel(this, wxID_ANY))
{
    this->AddPage(m_overview, i18n("overview"));
    this->AddPage(m_files,    i18n("files"));
    this->AddPage(m_peers,    i18n("peers"));
    this->AddPage(m_trackers, i18n("trackers"));
}

TorrentDetailsView::~TorrentDetailsView()
{
}

wxSize TorrentDetailsView::GetMinSize() const
{
    return FromDIP(wxSize(400, 100));
}

void TorrentDetailsView::Refresh(std::map<lt::info_hash_t, BitTorrent::TorrentHandle*> const& torrents)
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

void TorrentDetailsView::Reset()
{
    m_overview->Reset();
    m_files->Reset();
    m_peers->Reset();
    m_trackers->Reset();
}
