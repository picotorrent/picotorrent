#include "torrentcontextmenu.hpp"

#include "sessionstate.hpp"
#include "translator.hpp"
#include "utils.hpp"

#include <filesystem>
#include <sstream>

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <wx/clipbrd.h>
#include <wx/filedlg.h>

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using pt::TorrentContextMenu;

BEGIN_EVENT_TABLE(TorrentContextMenu, wxMenu)
    EVT_MENU(ptID_PAUSE, TorrentContextMenu::Pause)
    EVT_MENU(ptID_RESUME, TorrentContextMenu::Resume)
    EVT_MENU(ptID_RESUME_FORCE, TorrentContextMenu::Resume)
    EVT_MENU(ptID_QUEUE_UP, TorrentContextMenu::QueueUp)
    EVT_MENU(ptID_QUEUE_DOWN, TorrentContextMenu::QueueDown)
    EVT_MENU(ptID_QUEUE_TOP, TorrentContextMenu::QueueTop)
    EVT_MENU(ptID_QUEUE_BOTTOM, TorrentContextMenu::QueueBottom)
    EVT_MENU(ptID_MOVE, TorrentContextMenu::Move)
    EVT_MENU(ptID_REMOVE, TorrentContextMenu::Remove)
    EVT_MENU(ptID_COPY_INFO_HASH, TorrentContextMenu::CopyInfoHash)
    EVT_MENU(ptID_OPEN_IN_EXPLORER, TorrentContextMenu::OpenInExplorer)
    EVT_MENU(ptID_FORCE_RECHECK, TorrentContextMenu::ForceRecheck)
    EVT_MENU(ptID_FORCE_REANNOUNCE, TorrentContextMenu::ForceReannounce)
    EVT_MENU(ptID_SEQUENTIAL_DOWNLOAD, TorrentContextMenu::SequentialDownload)
END_EVENT_TABLE()

TorrentContextMenu::TorrentContextMenu(
    wxWindow* parent,
    std::shared_ptr<pt::Translator> tr,
    std::shared_ptr<pt::SessionState> state)
    : m_state(state),
    m_trans(tr),
    m_parent(parent)
{
    wxMenu* queueMenu = new wxMenu();
    queueMenu->Append(ptID_QUEUE_UP, i18n(tr, "up"));
    queueMenu->Append(ptID_QUEUE_DOWN, i18n(tr, "down"));
    queueMenu->AppendSeparator();
    queueMenu->Append(ptID_QUEUE_TOP, i18n(tr, "top"));
    queueMenu->Append(ptID_QUEUE_BOTTOM, i18n(tr, "bottom"));

    bool allPaused = std::all_of(
        m_state->selected_torrents.begin(),
        m_state->selected_torrents.end(),
        [this](lt::torrent_handle const& th)
    {
        lt::torrent_status ts = th.status();
        return (ts.flags & lt::torrent_flags::paused) == lt::torrent_flags::paused
            && !((ts.flags & lt::torrent_flags::auto_managed) == lt::torrent_flags::auto_managed);
    });

    bool allNotPaused = std::all_of(
        m_state->selected_torrents.begin(),
        m_state->selected_torrents.end(),
        [this](lt::torrent_handle const& th)
    {
        lt::torrent_status ts = th.status();
        return !((ts.flags & lt::torrent_flags::paused) == lt::torrent_flags::paused
            && !((ts.flags & lt::torrent_flags::auto_managed) == lt::torrent_flags::auto_managed));
    });

    wxMenuItem* resume = Append(ptID_RESUME, i18n(tr, "resume"));
    wxMenuItem* resumeForced = Append(ptID_RESUME_FORCE, i18n(tr, "resume_force"));
    wxMenuItem* pause = Append(ptID_PAUSE, i18n(tr, "pause"));

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
    Append(ptID_FORCE_REANNOUNCE, i18n(tr, "force_reannounce"));
    Append(ptID_FORCE_RECHECK, i18n(tr, "force_recheck"));

    if (m_state->selected_torrents.size() == 1)
    {
        const lt::torrent_handle& th = m_state->selected_torrents.at(0);
        bool isSequential = (th.flags() & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download;

        wxMenuItem* item = Append(ptID_SEQUENTIAL_DOWNLOAD, i18n(tr, "sequential_download"));
        item->SetCheckable(true);
        item->Check(isSequential);
    }

    AppendSeparator();
    Append(ptID_MOVE, i18n(tr, "move"));
    Append(ptID_REMOVE, i18n(tr, "remove"));
    AppendSeparator();
    AppendSubMenu(queueMenu, i18n(tr, "queuing"));
    AppendSeparator();
    Append(ptID_COPY_INFO_HASH, i18n(tr, "copy_info_hash"));
    Append(ptID_OPEN_IN_EXPLORER, i18n(tr, "open_in_explorer"));
}

void TorrentContextMenu::CopyInfoHash(wxCommandEvent& WXUNUSED(event))
{
    std::stringstream ss;

    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        ss << "," << th.info_hash();
    }

    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(ss.str().substr(1)));
        wxTheClipboard->Close();
    }
}

void TorrentContextMenu::ForceReannounce(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.force_reannounce();
    }
}

void TorrentContextMenu::ForceRecheck(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.force_recheck();

        lt::torrent_status ts = th.status();

        bool paused = ((ts.flags & lt::torrent_flags::paused)
            && !(ts.flags & lt::torrent_flags::auto_managed));

        if (paused)
        {
            th.resume();
            m_state->pause_after_checking.push_back(ts.info_hash);
        }
    }
}

void TorrentContextMenu::Move(wxCommandEvent& WXUNUSED(event))
{
    wxDirDialog dlg(
        m_parent,
        i18n(m_trans, "select_destination"),
        wxEmptyString,
        wxDD_DIR_MUST_EXIST);

    if (dlg.ShowModal() != wxID_OK)
    {
        return;
    }

    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.move_storage(dlg.GetPath().ToStdString());
    }
}

void TorrentContextMenu::OpenInExplorer(wxCommandEvent& WXUNUSED(event))
{
    lt::torrent_handle const& th = m_state->selected_torrents.front();
    lt::torrent_status ts = th.status();

    fs::path savePath = wxString::FromUTF8(ts.save_path).ToStdWstring();
    fs::path path = savePath / wxString::FromUTF8(ts.name).ToStdWstring();

    Utils::OpenAndSelect(path);
}

void TorrentContextMenu::Pause(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.unset_flags(lt::torrent_flags::auto_managed);
        th.pause(lt::torrent_handle::graceful_pause);
    }
}

void TorrentContextMenu::Remove(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        m_state->session->remove_torrent(th);
    }

    m_state->selected_torrents.clear();
}

void TorrentContextMenu::Resume(wxCommandEvent& event)
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        switch (event.GetId())
        {
        case ptID_RESUME:
            th.set_flags(lt::torrent_flags::auto_managed);
            break;
        case ptID_RESUME_FORCE:
            th.unset_flags(lt::torrent_flags::auto_managed);
            break;
        }

        th.clear_error();
        th.resume();
    }
}

void TorrentContextMenu::QueueUp(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.queue_position_up();
    }
}

void TorrentContextMenu::QueueDown(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.queue_position_down();
    }
}

void TorrentContextMenu::QueueTop(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.queue_position_top();
    }
}

void TorrentContextMenu::QueueBottom(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.queue_position_bottom();
    }
}

void TorrentContextMenu::SequentialDownload(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        bool isSequential = (th.flags() & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download;

        if (isSequential)
        {
            th.unset_flags(lt::torrent_flags::sequential_download);
        }
        else
        {
            th.set_flags(lt::torrent_flags::sequential_download);
        }
    }
}
