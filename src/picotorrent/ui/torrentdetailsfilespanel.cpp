#include "torrentdetailsfilespanel.hpp"

#include <wx/sizer.h>

#include "../bittorrent/torrenthandle.hpp"
#include "../bittorrent/torrentstatus.hpp"
#include "models/filestoragemodel.hpp"
#include "torrentfilelistview.hpp"
#include "translator.hpp"

using pt::UI::TorrentDetailsFilesPanel;

TorrentDetailsFilesPanel::TorrentDetailsFilesPanel(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id),
    m_fileList(new TorrentFileListView(this, wxID_ANY)),
    m_filesModel(new Models::FileStorageModel()),
    m_torrent(nullptr)
{
    m_fileList->AssociateModel(m_filesModel);
    m_filesModel->DecRef();

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_fileList, 1, wxEXPAND);
    this->SetSizerAndFit(mainSizer);
}

void TorrentDetailsFilesPanel::Refresh(BitTorrent::TorrentHandle* torrent)
{
    if (!torrent->IsValid())
    {
        return;
    }

    auto status = torrent->Status();

    if (auto tf = status.torrentFile.lock())
    {
        m_fileList->Freeze();

        if (m_torrent == nullptr || m_torrent->InfoHash() != torrent->InfoHash())
        {
            m_torrent = torrent;
            m_filesModel->RebuildTree(tf);
            m_fileList->Expand(m_filesModel->GetRootItem());
        }

        std::vector<int64_t> progress;
        torrent->FileProgress(progress, 0);

        m_filesModel->UpdatePriorities(m_torrent->GetFilePriorities());
        m_filesModel->UpdateProgress(progress);

        m_fileList->Thaw();
    }
}

void TorrentDetailsFilesPanel::Reset()
{
    m_torrent = nullptr;
    m_filesModel->ClearNodes();
    m_filesModel->Cleared();
}
