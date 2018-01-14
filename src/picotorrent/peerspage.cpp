#include "peerspage.hpp"

#include "peersviewmodel.hpp"
#include "translator.hpp"

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/dataview.h>

namespace lt = libtorrent;
using pt::PeersPage;

PeersPage::PeersPage(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, id),
    m_peersView(new wxDataViewCtrl(this, wxID_ANY)),
    m_viewModel(new PeersViewModel())
{
    m_peersView->AppendTextColumn(i18n(tr, "ip"), PeersViewModel::Columns::IP, wxDATAVIEW_CELL_INERT, 110);
    m_peersView->AppendTextColumn(i18n(tr, "client"), PeersViewModel::Columns::Client, wxDATAVIEW_CELL_INERT, 140);
    m_peersView->AppendTextColumn(i18n(tr, "flags"), PeersViewModel::Columns::Flags, wxDATAVIEW_CELL_INERT, 80);
    m_peersView->AppendTextColumn(i18n(tr, "dl"), PeersViewModel::Columns::DownloadRate, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
    m_peersView->AppendTextColumn(i18n(tr, "ul"), PeersViewModel::Columns::UploadRate, wxDATAVIEW_CELL_INERT, 80, wxALIGN_RIGHT);
    m_peersView->AppendProgressColumn(i18n(tr, "progress"), PeersViewModel::Columns::Progress, wxDATAVIEW_CELL_INERT, 100);

    m_peersView->AssociateModel(m_viewModel);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_peersView, 1, wxEXPAND);

    this->SetSizer(sizer);
}

void PeersPage::Clear()
{
    m_viewModel->Clear();
}

void PeersPage::Update(lt::torrent_status const& ts)
{
    m_viewModel->Update(ts);
}
