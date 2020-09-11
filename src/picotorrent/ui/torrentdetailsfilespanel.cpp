#include "torrentdetailsfilespanel.hpp"

#include <wx/sizer.h>

#include "../bittorrent/torrenthandle.hpp"
#include "../bittorrent/torrentstatus.hpp"
#include "models/filestoragemodel.hpp"
#include "torrentfilelistview.hpp"
#include "translator.hpp"

using pt::UI::TorrentDetailsFilesPanel;

enum
{
    ptID_CONTEXT_MENU_DO_NOT_DOWNLOAD,
    ptID_CONTEXT_MENU_LOW,
    ptID_CONTEXT_MENU_NORMAL,
    ptID_CONTEXT_MENU_MAXIMUM,
};

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

    this->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &TorrentDetailsFilesPanel::ShowFileContextMenu, this, wxID_ANY);
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

        if (m_torrent == nullptr
            || m_torrent->InfoHash() != torrent->InfoHash()
            || m_torrentPrevFileCount != tf->num_files())
        {
            m_torrent = torrent;
            m_torrentPrevFileCount = tf->num_files();

            m_filesModel->RebuildTree(tf);

            wxDataViewItemArray children;
            m_filesModel->GetChildren(m_filesModel->GetRootItem(), children);
            for (auto const& child : children) { m_fileList->Expand(child); }
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

void TorrentDetailsFilesPanel::ShowFileContextMenu(wxCommandEvent& evt)
{
    wxDataViewItemArray items;
    m_fileList->GetSelections(items);

    if (items.IsEmpty())
    {
        return;
    }

    auto priorities = m_torrent->GetFilePriorities();
    auto fileIndices = m_filesModel->GetFileIndices(items);
    auto firstPrio = priorities.size() > 0
        ? priorities[static_cast<int>(fileIndices[0])]
        : lt::default_priority;

    auto allSamePrio = std::all_of(
        fileIndices.begin(),
        fileIndices.end(),
        [&](lt::file_index_t i)
        {
            int i32 = static_cast<int>(i);
            auto p = priorities.size() >= i32 + size_t(1)
                ? priorities[i32]
                : lt::default_priority;
            return firstPrio == p;
        });

    wxMenu* prioMenu = new wxMenu();
    prioMenu->AppendCheckItem(ptID_CONTEXT_MENU_MAXIMUM, i18n("maximum"))
        ->Check(allSamePrio && firstPrio == lt::top_priority);
    prioMenu->AppendCheckItem(ptID_CONTEXT_MENU_NORMAL, i18n("normal"))
        ->Check(allSamePrio && firstPrio == lt::default_priority);
    prioMenu->AppendCheckItem(ptID_CONTEXT_MENU_LOW, i18n("low"))
        ->Check(allSamePrio && firstPrio == lt::low_priority);
    prioMenu->AppendSeparator();
    prioMenu->AppendCheckItem(ptID_CONTEXT_MENU_DO_NOT_DOWNLOAD, i18n("do_not_download"))
        ->Check(allSamePrio && firstPrio == lt::dont_download);

    wxMenu menu;
    menu.AppendSubMenu(prioMenu, i18n("priority"));
    menu.Bind(
        wxEVT_MENU,
        [this, &fileIndices, &priorities](wxCommandEvent& evt)
        {
            auto set = [&fileIndices, &priorities](lt::download_priority_t p)
            {
                for (lt::file_index_t idx : fileIndices)
                {
                    int i32 = static_cast<int>(idx);
                    if (priorities.size() <= i32)
                    {
                        priorities.resize(size_t(i32) + 1, lt::default_priority);
                    }

                    priorities.at(i32) = p;
                }
            };

            switch (evt.GetId())
            {
            case ptID_CONTEXT_MENU_DO_NOT_DOWNLOAD:
                set(lt::dont_download);
                break;
            case ptID_CONTEXT_MENU_LOW:
                set(lt::low_priority);
                break;
            case ptID_CONTEXT_MENU_MAXIMUM:
                set(lt::top_priority);
                break;
            case ptID_CONTEXT_MENU_NORMAL:
                set(lt::default_priority);
                break;
            }

            m_torrent->SetFilePriorities(priorities);
        });

    PopupMenu(&menu);
}
