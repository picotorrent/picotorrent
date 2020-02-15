#include "addtorrentdialog.hpp"
#include "ui_addtorrentdialog.h"

#include <filesystem>
#include <sstream>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupbox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

#include "filestorageitemmodel.hpp"
#include "translator.hpp"
#include "core/database.hpp"
#include "core/utils.hpp"

namespace fs = std::experimental::filesystem;
namespace lt = libtorrent;
using pt::AddTorrentDialog;

AddTorrentDialog::AddTorrentDialog(std::vector<lt::add_torrent_params>& params, std::shared_ptr<pt::Database> db, QWidget* parent)
    : QDialog(parent),
    m_params(params),
    m_db(db),
    m_ui(new Ui::AddTorrentDialog())
{
    m_ui->setupUi(this);

    // translate
    m_ui->fileGroup->setTitle(i18n("file"));
    m_ui->savePathBrowse->setText(i18n("browse"));
    m_ui->savePathLabel->setText(i18n("save_path"));
    m_ui->sequentialDownload->setText(i18n("sequential_download"));
    m_ui->startTorrent->setText(i18n("start_torrent"));
    m_ui->storageGroup->setTitle(i18n("storage"));
    m_ui->torrentGroup->setTitle(i18n("torrent"));
    m_ui->torrentCommentLabel->setText(i18n("comment"));
    m_ui->torrentInfoHashLabel->setText(i18n("info_hash"));
    m_ui->torrentNameLabel->setText(i18n("name"));
    m_ui->torrentSizeLabel->setText(i18n("size"));

    m_filesModel = new FileStorageItemModel();
    m_torrentContextMenu = new QMenu(this);

    m_ui->torrentFiles->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ui->torrentFiles->setModel(m_filesModel);
    m_ui->torrentFiles->hideColumn(FileStorageItemModel::Columns::Progress);
    m_ui->torrentFiles->setColumnWidth(FileStorageItemModel::Columns::Name, 240);
    m_ui->torrentFiles->setColumnWidth(FileStorageItemModel::Columns::Size, 60);
    m_ui->torrentFiles->setSelectionMode(QTreeView::ExtendedSelection);

    // Menu
    QMenu* priorities = m_torrentContextMenu->addMenu(i18n("priority"));
    priorities->addAction(i18n("maximum"))
        ->setData(static_cast<uint8_t>(lt::top_priority));
    priorities->addAction(i18n("normal"))
        ->setData(static_cast<uint8_t>(lt::default_priority));
    priorities->addAction(i18n("low"))
        ->setData(static_cast<uint8_t>(lt::low_priority));
    priorities->addSeparator();
    priorities->addAction(i18n("do_not_download"))
        ->setData(static_cast<uint8_t>(lt::dont_download));

    QObject::connect(m_ui->fileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     this,            &AddTorrentDialog::onTorrentIndexChanged);

    QObject::connect(m_ui->savePath, &QComboBox::editTextChanged,
                     this,           &AddTorrentDialog::onTorrentSavePathChanged);

    QObject::connect(m_ui->savePathBrowse, &QPushButton::clicked,
                     this,                 &AddTorrentDialog::onTorrentSavePathBrowse);

    QObject::connect(m_ui->sequentialDownload, &QCheckBox::stateChanged,
                     this,                     &AddTorrentDialog::onTorrentSequentialDownloadChanged);

    QObject::connect(m_ui->startTorrent, &QCheckBox::stateChanged,
                     this,               &AddTorrentDialog::onTorrentStartChanged);

    QObject::connect(m_ui->torrentFiles, &QTreeView::customContextMenuRequested,
                     this,               &AddTorrentDialog::onTorrentFileContextMenu);

    QObject::connect(m_torrentContextMenu, &QMenu::triggered,
                     this,                 &AddTorrentDialog::onSetTorrentFilePriorities);

    QObject::connect(m_ui->buttons, &QDialogButtonBox::accepted,
                     this,          &QDialog::accept);

    QObject::connect(m_ui->buttons, &QDialogButtonBox::accepted,
                     this,          &AddTorrentDialog::updateHistory);

    QObject::connect(m_ui->buttons, &QDialogButtonBox::rejected,
                     this,          &QDialog::reject);

    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    this->setMinimumWidth(430);
    this->setWindowFlags(flags);
    this->setWindowTitle(i18n("add_torrent_s"));

    // Load save path history
    auto stmt = m_db->statement("SELECT path FROM path_history WHERE type = 'add_torrent_dialog' ORDER BY timestamp DESC LIMIT 5");

    while (stmt->read())
    {
        m_ui->savePath->addItem(
            QString::fromStdString(stmt->getString(0)));
    }

    // Add torrents

    for (lt::add_torrent_params const& p : m_params)
    {
        m_ui->fileCombo->addItem(getDisplayName(p));
    }
}

AddTorrentDialog::~AddTorrentDialog()
{
    delete m_ui;
}

QString AddTorrentDialog::getDisplayComment(lt::add_torrent_params const& param)
{
    if (param.ti)
    {
        return QString::fromStdString(param.ti->comment());
    }

    return "-";
}

QString AddTorrentDialog::getDisplayHash(lt::add_torrent_params const& param)
{
    std::stringstream hash;

    if (param.ti)
    {
        if (param.ti->info_hash().has_v2())
        {
            hash << param.ti->info_hash().v2;
        }
        else
        {
            hash << param.ti->info_hash().v1;
        }
    }
    else if(param.info_hash.has_v1() || param.info_hash.has_v2())
    {
        if (param.info_hash.has_v2())
        {
            hash << param.info_hash.v2;
        }
        else
        {
            hash << param.info_hash.v1;
        }
    }
    else
    {
        hash << "-";
    }

    return QString::fromStdString(hash.str());
}

QString AddTorrentDialog::getDisplayName(lt::add_torrent_params const& param)
{
    if (param.ti)
    {
        return QString::fromStdString(param.ti->name());
    }

    if (param.name.size() > 0)
    {
        return QString::fromStdString(param.name);
    }

    std::stringstream hash;
    hash << param.info_hash;

    return QString::fromStdString(hash.str());
}

QString AddTorrentDialog::getDisplaySize(lt::add_torrent_params const& param)
{
    if (param.ti)
    {
        std::wstring humanSize = Utils::toHumanFileSize(param.ti->total_size());
        return QString::fromStdWString(humanSize);
    }

    return "-";
}

std::vector<lt::add_torrent_params> AddTorrentDialog::getParams()
{
    return m_params;
}

void AddTorrentDialog::refreshMetadata(std::shared_ptr<libtorrent::torrent_info>* ti)
{
    for (int i = 0; i < m_params.size(); i++)
    {
        if (m_params.at(i).info_hash == (*ti)->info_hash())
        {
            m_params.at(i).ti = (*ti);
            m_ui->fileCombo->setItemText(i, QString::fromStdString(m_params.at(i).ti->name()));

            if (i == m_ui->fileCombo->currentIndex())
            {
                onTorrentIndexChanged(i);
            }
        }
    }
}

void AddTorrentDialog::onSetTorrentFilePriorities(QAction* action)
{
    lt::add_torrent_params& params = m_params.at(m_ui->fileCombo->currentIndex());

    lt::download_priority_t prio = lt::download_priority_t{ static_cast<uint8_t>(action->data().toInt()) };
    auto const& indices = m_ui->torrentFiles->selectionModel()->selectedIndexes();
    auto fileIndices = m_filesModel->fileIndices(indices);

    for (int idx : fileIndices)
    {
        if (idx >= params.file_priorities.size())
        {
            params.file_priorities.resize(idx + 1, lt::default_priority);
        }

        params.file_priorities.at(idx) = prio;
    }

    m_filesModel->setPriorities(params.file_priorities);
}

void AddTorrentDialog::onTorrentFileContextMenu(QPoint const& point)
{
    QModelIndex idx = m_ui->torrentFiles->indexAt(point);

    if (idx.isValid())
    {
        m_torrentContextMenu->exec(m_ui->torrentFiles->viewport()->mapToGlobal(point));
    }
}

void AddTorrentDialog::onTorrentIndexChanged(int index)
{
    auto const& param = m_params.at(index);

    m_ui->torrentName->setText(getDisplayName(param));
    m_ui->torrentSize->setText(getDisplaySize(param));
    m_ui->torrentInfoHash->setText(getDisplayHash(param));
    m_ui->torrentComment->setText(getDisplayComment(param));
    m_ui->savePath->setEditText(QString::fromStdString(param.save_path));

    m_ui->sequentialDownload->setChecked(
        ((param.flags & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download));

    m_ui->startTorrent->setChecked(
        (param.flags & lt::torrent_flags::paused)
        && (param.flags & lt::torrent_flags::auto_managed));

    m_filesModel->setPriorities(param.file_priorities);

    if (!param.ti)
    {
        m_filesModel->clearTree();
        return;
    }

    m_filesModel->rebuildTree(param.ti);

    if (param.ti->num_files() > 1)
    {
        auto root = m_ui->torrentFiles->rootIndex();

        for (int i = 0; i < m_filesModel->rowCount(root); i++)
        {
            auto idx = m_filesModel->index(i, 0, root);
            m_ui->torrentFiles->expand(idx);
        }

        m_ui->torrentFiles->setRootIsDecorated(true);
    }
    else
    {
        // If the file name is the same as torrent name, this is a single file
        // torrent which downloads directly to the save path. In that case, hide
        // the root decorator.
        if (param.ti->files().file_path(lt::file_index_t{0}) == param.ti->name())
        {
            m_ui->torrentFiles->setRootIsDecorated(false);
        }
        else
        {
            m_ui->torrentFiles->expandAll();
            m_ui->torrentFiles->setRootIsDecorated(true);
        }
    }
}

void AddTorrentDialog::onTorrentSavePathBrowse()
{
    lt::add_torrent_params& params = m_params.at(m_ui->fileCombo->currentIndex());

    auto dlg = new QFileDialog(this);
    dlg->setDirectory(QString::fromStdString(params.save_path));
    dlg->setFileMode(QFileDialog::Directory);
    dlg->setOption(QFileDialog::ShowDirsOnly);
    dlg->open();

    QObject::connect(dlg, &QFileDialog::finished,
                     [this, dlg]()
                     {
                         QStringList files = dlg->selectedFiles();

                         if (files.size() > 0)
                         {
                             QString nativePath = QDir::toNativeSeparators(files.at(0));
                             m_ui->savePath->setEditText(nativePath);
                         }
                     });

    QObject::connect(dlg, &QFileDialog::finished,
                     dlg, &QFileDialog::deleteLater);
}

void AddTorrentDialog::onTorrentSavePathChanged(QString const& text)
{
    int index = m_ui->fileCombo->currentIndex();

    if (index < 0)
    {
        return;
    }

    lt::add_torrent_params& params = m_params.at(index);
    params.save_path = text.toStdString();
}

void AddTorrentDialog::onTorrentSequentialDownloadChanged(int state)
{
    lt::add_torrent_params& params = m_params.at(m_ui->fileCombo->currentIndex());

    switch (state)
    {
    case Qt::Unchecked:
        params.flags &= ~lt::torrent_flags::sequential_download;
        break;
    default:
        params.flags |= lt::torrent_flags::sequential_download;
        break;
    }
}

void AddTorrentDialog::onTorrentStartChanged(int state)
{
    lt::add_torrent_params& params = m_params.at(m_ui->fileCombo->currentIndex());

    switch (state)
    {
    case Qt::Unchecked:
        params.flags &= ~lt::torrent_flags::auto_managed;
        break;
    default:
        params.flags |= lt::torrent_flags::auto_managed;
        break;
    }
}

void AddTorrentDialog::updateHistory()
{
    for (int i = 0; i < m_params.size(); i++)
    {
        lt::add_torrent_params const& p = m_params.at(i);

        auto stmt = m_db->statement("INSERT INTO path_history (path, type, timestamp) VALUES(?, 'add_torrent_dialog', strftime('%s'))\n"
            "ON CONFLICT (path, type) DO UPDATE SET timestamp = excluded.timestamp;");
        stmt->bind(1, p.save_path);
        stmt->execute();
    }

    // Remove all entries except the last 5
    m_db->execute("DELETE FROM path_history WHERE id NOT IN (SELECT id FROM path_history WHERE type = 'add_torrent_dialog' ORDER BY timestamp DESC LIMIT 5)");
}
