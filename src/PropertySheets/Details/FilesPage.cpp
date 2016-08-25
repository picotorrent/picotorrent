#include "FilesPage.hpp"

#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include <strsafe.h>

#include "../../Commands/PrioritizeFilesCommand.hpp"
#include "../../Configuration.hpp"
#include "../../Models/TorrentFile.hpp"
#include "../../resources.h"
#include "../../Translator.hpp"
#include "../../UI/TorrentFileListView.hpp"

namespace lt = libtorrent;
using PropertySheets::Details::FilesPage;

FilesPage::FilesPage(const libtorrent::torrent_handle& th)
    : m_torrent(th)
{
    m_title = TRW("files");
    SetTitle(m_title.c_str());
}

std::vector<Models::TorrentFile> FilesPage::Map(const lt::torrent_handle& th)
{
    std::vector<Models::TorrentFile> result;

    std::vector<int64_t> progress;
    th.file_progress(progress, lt::torrent_handle::file_progress_flags_t::piece_granularity);

    auto files = th.torrent_file()->files();

    for (int i = 0; i < files.num_files(); i++)
    {
        Models::TorrentFile f{ i };
        f.name = TWS(files.file_path(i));
        f.priority = th.file_priority(i);
        f.progress = (float)progress.at(i) / files.file_size(i);
        f.size = files.file_size(i);

        result.push_back(f);
    }

    return result;
}

void FilesPage::OnDestroy()
{
    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_UNREGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);
}

BOOL FilesPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    // Set up UI
    m_filesList = std::make_unique<UI::TorrentFileListView>(
        GetDlgItem(ID_DETAILS_FILES_LIST),
        true);

    // Add files
    if (m_torrent.torrent_file())
    {
        for (auto& m : Map(m_torrent))
        {
            m_filesList->Add(m);
        }

        m_filesList->SetItemCount(m_torrent.torrent_file()->num_files());
    }

    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_REGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);

    return FALSE;
}

LRESULT FilesPage::OnPrioritizeFiles(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto cmd = reinterpret_cast<Commands::PrioritizeFilesCommand*>(lParam);
    auto files = Map(m_torrent);

    for (int idx : cmd->indices)
    {
        m_torrent.file_priority(idx, cmd->priority);
        
        files.at(idx).priority = cmd->priority;
        m_filesList->Update(files.at(idx));
    }

    return FALSE;
}

LRESULT FilesPage::OnTorrentUpdated(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    lt::sha1_hash* hash = reinterpret_cast<lt::sha1_hash*>(lParam);
    if (*hash != m_torrent.info_hash()) { return FALSE; }
    if (!m_torrent.torrent_file()) { return FALSE; }

    for (auto& m : Map(m_torrent))
    {
        m_filesList->Update(m);
    }

    auto sel = m_filesList->GetVisibleIndices();
    m_filesList->RedrawItems(sel.first, sel.second);

    return FALSE;
}
