#include "peerspage.hpp"

#include "peersviewmodel.hpp"
#include "translator.hpp"
#include "scaler.hpp"

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/dataview.h>

namespace lt = libtorrent;
using pt::PeersPage;

PeersPage::PeersPage(wxWindow* parent, wxWindowID id, std::shared_ptr<pt::Translator> tr)
    : wxPanel(parent, id),
    m_peersView(new PeersListView(this, ptID_TORRENT_LIST_VIEW, tr)),
    m_viewModel(new PeersViewModel())
{
    m_peersView->AssociateModel(m_viewModel);
    m_viewModel->DecRef();

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
    m_peersView->Sort();
}
