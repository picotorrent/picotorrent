#include "torrentcontextmenu.hpp"

#include <QClipboard>
#include <QGuiApplication>

#include "sessionstate.hpp"
#include "translator.hpp"
#include "utils.hpp"

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

namespace fs = std::experimental::filesystem;
namespace lt = libtorrent;

using pt::TorrentContextMenu;

TorrentContextMenu::TorrentContextMenu(QWidget* parent, std::shared_ptr<pt::SessionState> state)
    : m_state(state),
    m_parent(parent)
{
    m_pause = new QAction(i18n("pause"));
    m_resume = new QAction(i18n("resume"));
    m_resumeForce = new QAction(i18n("resume_force"));
    m_move = new QAction(i18n("move"));
    m_remove = new QAction(i18n("remove_torrent"));
    m_removeFiles = new QAction(i18n("remove_torrent_and_files"));
    m_queueUp = new QAction(i18n("up"));
    m_queueDown = new QAction(i18n("down"));
    m_queueTop = new QAction(i18n("top"));
    m_queueBottom = new QAction(i18n("bottom"));
    m_copyHash = new QAction(i18n("copy_info_hash"));
    m_openExplorer = new QAction(i18n("open_in_explorer"));
    m_forceRecheck = new QAction(i18n("force_recheck"));
    m_forceReannounce = new QAction(i18n("force_reannounce"));
    m_sequentialDownload = new QAction(i18n("sequential_download"));

    m_queueMenu = new QMenu(i18n("queuing"));
    m_queueMenu->addAction(m_queueUp);
    m_queueMenu->addAction(m_queueDown);
    m_queueMenu->addSeparator();
    m_queueMenu->addAction(m_queueTop);
    m_queueMenu->addAction(m_queueBottom);

    m_removeMenu = new QMenu(i18n("remove"));
    m_removeMenu->addAction(m_remove);
    m_removeMenu->addAction(m_removeFiles);

    this->addAction(m_pause);
    this->addAction(m_resume);
    this->addAction(m_resumeForce);
    this->addSeparator();
    this->addAction(m_forceReannounce);
    this->addAction(m_forceRecheck);

    if (state->selectedTorrents.size() == 1)
    {
        this->addAction(m_sequentialDownload);
    }

    this->addSeparator();
    this->addAction(m_move);
    this->addSeparator();
    this->addMenu(m_removeMenu);
    this->addSeparator();
    this->addMenu(m_queueMenu);
    this->addSeparator();
    this->addAction(m_copyHash);
    this->addAction(m_openExplorer);

    connect(m_remove,       &QAction::triggered, this, &TorrentContextMenu::remove);
    connect(m_removeFiles,  &QAction::triggered, this, &TorrentContextMenu::removeFiles);
    connect(m_queueUp,      &QAction::triggered, this, &TorrentContextMenu::queueUp);
    connect(m_queueDown,    &QAction::triggered, this, &TorrentContextMenu::queueDown);
    connect(m_queueTop,     &QAction::triggered, this, &TorrentContextMenu::queueTop);
    connect(m_queueBottom,  &QAction::triggered, this, &TorrentContextMenu::queueBottom);
    connect(m_copyHash,     &QAction::triggered, this, &TorrentContextMenu::copyInfoHash);
    connect(m_openExplorer, &QAction::triggered, this, &TorrentContextMenu::openExplorer);
}

void TorrentContextMenu::copyInfoHash()
{
    std::stringstream ss;

    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        ss << "," << hash;
    }

    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(QString::fromStdString(ss.str().substr(1)));
}

/*
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

    std::string path = dlg.GetPath().ToUTF8();

    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.move_storage(path);
    }
}
*/
void TorrentContextMenu::openExplorer()
{
    lt::sha1_hash const& hash = (*m_state->selectedTorrents.begin());
    lt::torrent_handle th = m_state->torrents.at(hash);
    lt::torrent_status ts = th.status();

    fs::path savePath = QString::fromStdString(ts.save_path).toStdWString();
    fs::path path = savePath / QString::fromStdString(ts.name).toStdWString();

    Utils::openAndSelect(path);
}
/*
void TorrentContextMenu::Pause(wxCommandEvent& WXUNUSED(event))
{
    for (lt::torrent_handle& th : m_state->selected_torrents)
    {
        th.unset_flags(lt::torrent_flags::auto_managed);
        th.pause(lt::torrent_handle::graceful_pause);
    }
}
*/
void TorrentContextMenu::remove()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        m_state->session->remove_torrent(m_state->torrents.at(hash));
    }

    // m_state->selectedTorrents.clear();
}

void TorrentContextMenu::removeFiles()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        m_state->session->remove_torrent(
            m_state->torrents.at(hash),
            lt::session_handle::delete_files);
    }

    // m_state->selectedTorrents.clear();
}

/*
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
*/
void TorrentContextMenu::queueUp()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        m_state->torrents.at(hash).queue_position_up();
    }
}

void TorrentContextMenu::queueDown()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        m_state->torrents.at(hash).queue_position_down();
    }
}

void TorrentContextMenu::queueTop()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        m_state->torrents.at(hash).queue_position_top();
    }
}

void TorrentContextMenu::queueBottom()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        m_state->torrents.at(hash).queue_position_bottom();
    }
}
/*
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
*/
