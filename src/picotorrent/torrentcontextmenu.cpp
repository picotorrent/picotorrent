#include "torrentcontextmenu.hpp"

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include <picotorrent/core/utils.hpp>

#include <QClipboard>
#include <QFileDialog>
#include <QGuiApplication>

#include "sessionstate.hpp"
#include "translator.hpp"

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

    connect(m_pause,           &QAction::triggered, this, &TorrentContextMenu::pause);
    connect(m_resume,          &QAction::triggered, this, &TorrentContextMenu::resume);
    connect(m_resumeForce,     &QAction::triggered, this, &TorrentContextMenu::resumeForce);
    connect(m_forceReannounce, &QAction::triggered, this, &TorrentContextMenu::forceReannounce);
    connect(m_forceRecheck,    &QAction::triggered, this, &TorrentContextMenu::forceRecheck);
    connect(m_move,            &QAction::triggered, this, &TorrentContextMenu::move);
    connect(m_remove,          &QAction::triggered, this, &TorrentContextMenu::remove);
    connect(m_removeFiles,     &QAction::triggered, this, &TorrentContextMenu::removeFiles);
    connect(m_queueUp,         &QAction::triggered, this, &TorrentContextMenu::queueUp);
    connect(m_queueDown,       &QAction::triggered, this, &TorrentContextMenu::queueDown);
    connect(m_queueTop,        &QAction::triggered, this, &TorrentContextMenu::queueTop);
    connect(m_queueBottom,     &QAction::triggered, this, &TorrentContextMenu::queueBottom);
    connect(m_copyHash,        &QAction::triggered, this, &TorrentContextMenu::copyInfoHash);
    connect(m_openExplorer,    &QAction::triggered, this, &TorrentContextMenu::openExplorer);
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

void TorrentContextMenu::forceReannounce()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        m_state->torrents.at(hash).force_reannounce();
    }
}

void TorrentContextMenu::forceRecheck()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        lt::torrent_handle& th = m_state->torrents.at(hash);
        th.force_recheck();

        lt::torrent_status ts = th.status();

        bool paused = ((ts.flags & lt::torrent_flags::paused)
            && !(ts.flags & lt::torrent_flags::auto_managed));

        if (paused)
        {
            th.resume();
            m_state->pauseAfterChecking.insert(hash);
        }
    }
}

void TorrentContextMenu::move()
{
    auto dlg = new QFileDialog(m_parent);
    dlg->setFileMode(QFileDialog::Directory);
    dlg->setOption(QFileDialog::ShowDirsOnly);
    dlg->open();

    QObject::connect(
        dlg, &QDialog::finished,
        [dlg, this](int result)
        {
            if (result)
            {
                std::string path = dlg->selectedFiles().first().toStdString();

                for (lt::sha1_hash const& hash : m_state->selectedTorrents)
                {
                    m_state->torrents.at(hash).move_storage(path);
                }
            }

            dlg->deleteLater();
        });
}

void TorrentContextMenu::openExplorer()
{
    lt::sha1_hash const& hash = (*m_state->selectedTorrents.begin());
    lt::torrent_handle th = m_state->torrents.at(hash);
    lt::torrent_status ts = th.status();

    fs::path savePath = QString::fromStdString(ts.save_path).toStdWString();
    fs::path path = savePath / QString::fromStdString(ts.name).toStdWString();

    Utils::openAndSelect(path);
}

void TorrentContextMenu::pause()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        lt::torrent_handle& th = m_state->torrents.at(hash);
        th.unset_flags(lt::torrent_flags::auto_managed);
        th.pause(lt::torrent_handle::graceful_pause);
    }
}

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

void TorrentContextMenu::resume()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        lt::torrent_handle& th = m_state->torrents.at(hash);
        th.set_flags(lt::torrent_flags::auto_managed);
        th.clear_error();
        th.resume();
    }
}

void TorrentContextMenu::resumeForce()
{
    for (lt::sha1_hash const& hash : m_state->selectedTorrents)
    {
        lt::torrent_handle& th = m_state->torrents.at(hash);
        th.unset_flags(lt::torrent_flags::auto_managed);
        th.clear_error();
        th.resume();
    }
}

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
