#include "torrentcontextmenu.hpp"

#include <filesystem>

#include <wx/clipbrd.h>

#include "../bittorrent/torrenthandle.hpp"
#include "../bittorrent/torrentstatus.hpp"
#include "../core/utils.hpp"
#include "translator.hpp"

namespace fs = std::filesystem;
using pt::UI::TorrentContextMenu;

TorrentContextMenu::TorrentContextMenu(wxWindow* parent, std::vector<BitTorrent::TorrentHandle*> const& selectedTorrents)
    : wxMenu(),
    m_parent(parent)
{
    wxMenu* queueMenu = new wxMenu();
    queueMenu->Append(ptID_QUEUE_UP, i18n("up"));
    queueMenu->Append(ptID_QUEUE_DOWN, i18n("down"));
    queueMenu->AppendSeparator();
    queueMenu->Append(ptID_QUEUE_TOP, i18n("top"));
    queueMenu->Append(ptID_QUEUE_BOTTOM, i18n("bottom"));

    wxMenu* removeMenu = new wxMenu();
    removeMenu->Append(ptID_REMOVE, i18n("remove_torrent"));
    removeMenu->Append(ptID_REMOVE_FILES, i18n("remove_torrent_and_files"));

    bool allPaused = std::all_of(
        selectedTorrents.begin(),
        selectedTorrents.end(),
        [](BitTorrent::TorrentHandle* torrent)
        {
            return torrent->Status().paused;
        });

    bool allNotPaused = std::all_of(
        selectedTorrents.begin(),
        selectedTorrents.end(),
        [](BitTorrent::TorrentHandle* torrent)
        {
            return !torrent->Status().paused;
        });

    wxMenuItem* resume = Append(ptID_RESUME, i18n("resume"));
    wxMenuItem* resumeForced = Append(ptID_RESUME_FORCE, i18n("resume_force"));
    wxMenuItem* pause = Append(ptID_PAUSE, i18n("pause"));

    if (allPaused)
    {
        Delete(pause);
    }

    if (allNotPaused)
    {
        Delete(resume);
        Delete(resumeForced);
    }

    AppendSeparator();
    Append(ptID_FORCE_REANNOUNCE, i18n("force_reannounce"));
    Append(ptID_FORCE_RECHECK, i18n("force_recheck"));

    if (selectedTorrents.size() == 1)
    {
        /*const lt::torrent_handle& th = m_state->selected_torrents.at(0);
        bool isSequential = (th.flags() & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download;

        wxMenuItem* item = Append(ptID_SEQUENTIAL_DOWNLOAD, i18n(tr, "sequential_download"));
        item->SetCheckable(true);
        item->Check(isSequential);
        */
    }

    AppendSeparator();
    Append(ptID_MOVE, i18n("move"));
    AppendSubMenu(removeMenu, i18n("remove"));
    AppendSeparator();
    AppendSubMenu(queueMenu, i18n("queuing"));
    AppendSeparator();
    Append(ptID_COPY_INFO_HASH, i18n("copy_info_hash"));
    Append(ptID_OPEN_IN_EXPLORER, i18n("open_in_explorer"));

    // Bind events
    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto t : selectedTorrents) { t->Resume(); } },
        TorrentContextMenu::ptID_RESUME);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto t : selectedTorrents) { t->ResumeForce(); } },
        TorrentContextMenu::ptID_RESUME_FORCE);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto t : selectedTorrents) { t->Pause(); } },
        TorrentContextMenu::ptID_PAUSE);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&)
        {
            wxDirDialog dlg(
                m_parent,
                i18n("select_destination"),
                wxEmptyString,
                wxDD_DIR_MUST_EXIST);

            if (dlg.ShowModal() != wxID_OK)
            {
                return;
            }

            for (auto torrent : selectedTorrents)
            {
                torrent->MoveStorage(dlg.GetPath().ToStdString());
            }
        },
        TorrentContextMenu::ptID_MOVE);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto torrent : selectedTorrents) { torrent->Remove(); } },
        TorrentContextMenu::ptID_REMOVE);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&)
        {
            if (wxMessageBox(
                i18n("confirm_remove_description"),
                i18n("confirm_remove"),
                wxOK | wxCANCEL | wxICON_INFORMATION,
                m_parent) != wxOK) { return; }

            for (auto torrent : selectedTorrents) { torrent->Remove(); }
        },
        TorrentContextMenu::ptID_REMOVE_FILES);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto torrent : selectedTorrents) { torrent->QueueUp(); } },
        TorrentContextMenu::ptID_QUEUE_UP);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto torrent : selectedTorrents) { torrent->QueueDown(); } },
        TorrentContextMenu::ptID_QUEUE_DOWN);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto torrent : selectedTorrents) { torrent->QueueTop(); } },
        TorrentContextMenu::ptID_QUEUE_TOP);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto torrent : selectedTorrents) { torrent->QueueBottom(); } },
        TorrentContextMenu::ptID_QUEUE_BOTTOM);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&)
        {
            std::stringstream ss;
            for (auto torrent : selectedTorrents)
            {
                ss << ",";
                if (torrent->InfoHash().has_v2()) { ss << torrent->InfoHash().v2; }
                if (torrent->InfoHash().has_v1()) { ss << torrent->InfoHash().v1; }
            }

            if (wxTheClipboard->Open())
            {
                wxTheClipboard->SetData(new wxTextDataObject(ss.str().substr(1)));
                wxTheClipboard->Close();
            }
        },
        TorrentContextMenu::ptID_COPY_INFO_HASH);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&)
        {
            auto torrent = selectedTorrents.front();
            auto status = torrent->Status();

            fs::path savePath = wxString::FromUTF8(status.savePath).ToStdWstring();
            fs::path path = savePath / wxString::FromUTF8(status.name).ToStdWstring();

            Utils::openAndSelect(path);
        },
        TorrentContextMenu::ptID_OPEN_IN_EXPLORER);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto torrent : selectedTorrents) { torrent->ForceRecheck(); } },
        TorrentContextMenu::ptID_FORCE_RECHECK);

    Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&) { for (auto torrent : selectedTorrents) { torrent->ForceReannounce(); } },
        TorrentContextMenu::ptID_FORCE_REANNOUNCE);

        // TODO ptID_SEQUENTIAL_DOWNLOAD
}
