#include "torrentdetailsview.hpp"

#include "filespage.hpp"
#include "overviewpage.hpp"
#include "peerspage.hpp"
#include "sessionstate.hpp"
#include "trackerspage.hpp"
#include "translator.hpp"

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/notebook.h>

namespace lt = libtorrent;
using pt::TorrentDetailsView;

TorrentDetailsView::TorrentDetailsView(wxWindow* parent,
    std::shared_ptr<pt::Translator> tran,
    std::shared_ptr<pt::SessionState> state)
    : wxPanel(parent),
    m_state(state),
    m_notebook(new wxNotebook(this, wxID_ANY)),
    m_overview(new OverviewPage(m_notebook, wxID_ANY, tran)),
    m_files(new FilesPage(m_notebook, wxID_ANY, tran)),
    m_peers(new PeersPage(m_notebook, wxID_ANY, tran)),
    m_trackers(new TrackersPage(m_notebook, wxID_ANY, tran))
{
    m_notebook->AddPage(m_overview, i18n(tran, "overview"));
    m_notebook->AddPage(m_files, i18n(tran, "files"));
    m_notebook->AddPage(m_peers, i18n(tran, "peers"));
    m_notebook->AddPage(m_trackers, i18n(tran, "trackers"));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);    
    sizer->Add(m_notebook, 1, wxEXPAND, 0);

    this->SetSizer(sizer);
}

void TorrentDetailsView::Clear()
{
    m_overview->Clear();
    m_files->Clear();
    m_peers->Clear();
    m_trackers->Clear();
}

void TorrentDetailsView::Update()
{
    if (m_state->selected_torrents.size() != 1)
    {
        return;
    }

    lt::torrent_handle th = m_state->selected_torrents.front();
    lt::torrent_status ts = th.status();

    m_overview->Update(ts);
    m_files->Update(ts);
    m_peers->Update(ts);
    m_trackers->Update(ts);
}
