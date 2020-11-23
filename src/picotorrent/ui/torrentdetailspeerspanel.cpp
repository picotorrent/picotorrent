#include "torrentdetailspeerspanel.hpp"

#include <libtorrent/peer_info.hpp>
#include <wx/dataview.h>
#include <wx/sizer.h>

#include "../bittorrent/torrenthandle.hpp"
#include "../bittorrent/torrentstatus.hpp"
#include "models/peerlistmodel.hpp"
#include "translator.hpp"

using pt::UI::TorrentDetailsPeersPanel;
using pt::UI::Models::PeerListModel;

TorrentDetailsPeersPanel::TorrentDetailsPeersPanel(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id),
    m_peersView(new wxDataViewCtrl(this, wxID_ANY)),
    m_peersModel(new PeerListModel())
{
    m_peersView->AppendTextColumn(i18n("ip"), PeerListModel::Column::IP, wxDATAVIEW_CELL_INERT, FromDIP(110));
    m_peersView->AppendTextColumn(i18n("client"), PeerListModel::Column::Client, wxDATAVIEW_CELL_INERT, FromDIP(140));
    m_peersView->AppendTextColumn(i18n("flags"), PeerListModel::Column::Flags, wxDATAVIEW_CELL_INERT, FromDIP(80));
    m_peersView->AppendTextColumn(i18n("dl"), PeerListModel::Column::DownloadRate, wxDATAVIEW_CELL_INERT, FromDIP(80), wxALIGN_RIGHT);
    m_peersView->AppendTextColumn(i18n("ul"), PeerListModel::Column::UploadRate, wxDATAVIEW_CELL_INERT, FromDIP(80), wxALIGN_RIGHT);
    m_peersView->AppendProgressColumn(i18n("progress"), PeerListModel::Column::Progress, wxDATAVIEW_CELL_INERT, FromDIP(100), wxALIGN_LEFT);

    // Ugly hack to prevent the last "real" column from stretching.
    m_peersView->AppendColumn(new wxDataViewColumn(wxEmptyString, new wxDataViewTextRenderer(), PeerListModel::Column::_Max, 0));

    m_peersView->AssociateModel(m_peersModel);
    m_peersModel->DecRef();

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_peersView, 1, wxEXPAND);
    this->SetSizerAndFit(mainSizer);
}

void TorrentDetailsPeersPanel::Refresh(BitTorrent::TorrentHandle* torrent)
{
    if (!torrent->IsValid())
    {
        this->Reset();
        return;
    }

    std::vector<lt::peer_info> peers;
    torrent->GetPeerInfo(peers);

    m_peersModel->Update(peers);
}

void TorrentDetailsPeersPanel::Reset()
{
    m_peersModel->ResetPeers();
}
