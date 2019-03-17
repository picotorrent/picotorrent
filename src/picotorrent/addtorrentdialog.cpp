#include "addtorrentdialog.hpp"

#include <filesystem>
#include <sstream>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>

#include <picotorrent/core/utils.hpp>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
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

namespace fs = std::experimental::filesystem;
namespace lt = libtorrent;
using pt::AddTorrentDialog;

AddTorrentDialog::AddTorrentDialog(QWidget* parent, std::vector<lt::add_torrent_params>& params)
    : QDialog(parent),
    m_params(params)
{
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    m_paramsList = new QComboBox();
    m_torrentName = new QLabel("-");
    m_torrentSize = new QLabel("-");
    m_torrentInfoHash = new QLabel("-");
    m_torrentComment = new QLabel("-");
    m_torrentSavePath = new QLineEdit();
    m_torrentSavePathBrowse = new QPushButton("Browse");
    m_torrentSequentialDownload = new QCheckBox("Sequential download");
    m_torrentStart = new QCheckBox("Start torrent");
    m_filesModel = new FileStorageItemModel();
    m_torrentContextMenu = new QMenu();
    m_ok = new QPushButton("OK");
    m_cancel = new QPushButton("Cancel");

    m_torrentFiles = new QTreeView();
    m_torrentFiles->setContextMenuPolicy(Qt::CustomContextMenu);
    m_torrentFiles->setModel(m_filesModel);
    m_torrentFiles->hideColumn(FileStorageItemModel::Columns::Progress);
    m_torrentFiles->setColumnWidth(FileStorageItemModel::Columns::Name, 240);
    m_torrentFiles->setColumnWidth(FileStorageItemModel::Columns::Size, 60);
    m_torrentFiles->setSelectionMode(QTreeView::ExtendedSelection);

    // Menu
    QMenu* priorities = m_torrentContextMenu->addMenu("Priority");
    priorities->addAction("Maximum")
        ->setData(static_cast<uint8_t>(lt::top_priority));
    priorities->addAction("Normal")
        ->setData(static_cast<uint8_t>(lt::default_priority));
    priorities->addAction("Low")
        ->setData(static_cast<uint8_t>(lt::low_priority));
    priorities->addSeparator();
    priorities->addAction("Do not download")
        ->setData(static_cast<uint8_t>(lt::dont_download));

    auto fileGroup = new QGroupBox("File");
    auto fileLayout = new QVBoxLayout();
    fileLayout->addWidget(m_paramsList);
    fileGroup->setLayout(fileLayout);

    auto torrentGroup = new QGroupBox("Torrent");
    auto torrentGrid = new QGridLayout();
    torrentGrid->addWidget(new QLabel("Name"), 0, 0);
    torrentGrid->addWidget(m_torrentName, 0, 1);
    torrentGrid->addWidget(new QLabel("Size"), 1, 0);
    torrentGrid->addWidget(m_torrentSize, 1, 1);
    torrentGrid->addWidget(new QLabel("Info hash"), 2, 0);
    torrentGrid->addWidget(m_torrentInfoHash, 2, 1);
    torrentGrid->addWidget(new QLabel("Comment"), 3, 0);
    torrentGrid->addWidget(m_torrentComment, 3, 1);
    torrentGrid->setColumnStretch(0, 1);
    torrentGrid->setColumnStretch(1, 2);
    torrentGroup->setLayout(torrentGrid);

    auto prefsGroup = new QGroupBox("Preferences");
    auto prefsGrid = new QGridLayout();
    prefsGrid->addWidget(new QLabel("Save path"), 0, 0);

    auto dirBrowseLayout = new QHBoxLayout();
    dirBrowseLayout->addWidget(m_torrentSavePath);
    dirBrowseLayout->addWidget(m_torrentSavePathBrowse);
    dirBrowseLayout->setStretch(0, 1);
    prefsGrid->addLayout(dirBrowseLayout, 0, 1);

    auto optsLayout = new QHBoxLayout();
    optsLayout->addWidget(m_torrentSequentialDownload);
    optsLayout->addWidget(m_torrentStart);
    prefsGrid->addLayout(optsLayout, 1, 1);

    prefsGrid->addWidget(m_torrentFiles, 2, 0, -1, -1);
    prefsGrid->setRowStretch(2, 1);

    prefsGroup->setLayout(prefsGrid);

    auto btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    btnLayout->addWidget(m_ok);
    btnLayout->addWidget(m_cancel);

    auto layout = new QVBoxLayout();
    layout->addWidget(fileGroup);
    layout->addWidget(torrentGroup);
    layout->addWidget(prefsGroup);
    layout->addLayout(btnLayout);
    layout->setStretch(2, 1);

    connect(
        m_paramsList,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &AddTorrentDialog::onTorrentIndexChanged);

    connect(
        m_torrentSavePath,
        &QLineEdit::textChanged,
        this,
        &AddTorrentDialog::onTorrentSavePathChanged);

    connect(
        m_torrentSavePathBrowse,
        &QPushButton::clicked,
        this,
        &AddTorrentDialog::onTorrentSavePathBrowse);

    connect(
        m_torrentSequentialDownload,
        &QCheckBox::stateChanged,
        this,
        &AddTorrentDialog::onTorrentSequentialDownloadChanged);

    connect(
        m_torrentStart,
        &QCheckBox::stateChanged,
        this,
        &AddTorrentDialog::onTorrentStartChanged);

    connect(
        m_torrentFiles,
        &QTreeView::customContextMenuRequested,
        this,
        &AddTorrentDialog::onTorrentFileContextMenu);

    connect(
        m_torrentContextMenu,
        &QMenu::triggered,
        this,
        &AddTorrentDialog::onSetTorrentFilePriorities);

    connect(
        m_cancel,
        &QPushButton::clicked,
        [this]() { done(QDialog::Rejected); });

    connect(
        m_ok,
        &QPushButton::clicked,
        [this]() { done(QDialog::Accepted); });

    this->setLayout(layout);
    this->setMinimumWidth(430);
    this->setWindowFlags(flags);
    this->setWindowTitle("Add torrent(s)");

    // Add torrents
    for (lt::add_torrent_params const& p : m_params)
    {
        m_paramsList->addItem(QString::fromStdString(p.ti->name()));
    }
}

std::vector<lt::add_torrent_params> AddTorrentDialog::getParams()
{
    return m_params;
}

void AddTorrentDialog::onSetTorrentFilePriorities(QAction* action)
{
    lt::add_torrent_params& params = m_params.at(m_paramsList->currentIndex());

    lt::download_priority_t prio = lt::download_priority_t{ static_cast<uint8_t>(action->data().toInt()) };
    auto const& indices = m_torrentFiles->selectionModel()->selectedIndexes();
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
    QModelIndex idx = m_torrentFiles->indexAt(point);

    if (idx.isValid())
    {
        m_torrentContextMenu->exec(m_torrentFiles->viewport()->mapToGlobal(point));
    }
}

void AddTorrentDialog::onTorrentIndexChanged(int index)
{
    auto const& param = m_params.at(index);

    std::stringstream hash;
    hash << param.ti->info_hash();

    m_torrentName->setText(QString::fromStdString(param.ti->name()));
    m_torrentSize->setText(QString::fromStdWString(Utils::toHumanFileSize(param.ti->total_size())));
    m_torrentInfoHash->setText(QString::fromStdString(hash.str()));
    m_torrentComment->setText(QString::fromStdString(param.ti->comment()));
    m_torrentSavePath->setText(QString::fromStdString(param.save_path));
    m_torrentSequentialDownload->setChecked(
        ((param.flags & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download));
    m_torrentStart->setChecked(
        (param.flags & lt::torrent_flags::paused)
        && (param.flags & lt::torrent_flags::auto_managed));

    m_filesModel->rebuildTree(param.ti);
    m_filesModel->setPriorities(param.file_priorities);

    if (param.ti->num_files() > 1)
    {
        auto root = m_torrentFiles->rootIndex();

        for (int i = 0; i < m_filesModel->rowCount(root); i++)
        {
            auto idx = m_filesModel->index(i, 0, root);
            m_torrentFiles->expand(idx);
        }

        m_torrentFiles->setRootIsDecorated(true);
    }
    else
    {
        // If the file name is the same as torrent name, this is a single file
        // torrent which downloads directly to the save path. In that case, hide
        // the root decorator.
        if (param.ti->files().file_path(lt::file_index_t{0}) == param.ti->name())
        {
            m_torrentFiles->setRootIsDecorated(false);
        }
        else
        {
            m_torrentFiles->expandAll();
            m_torrentFiles->setRootIsDecorated(true);
        }
    }
}

void AddTorrentDialog::onTorrentSavePathBrowse()
{
    lt::add_torrent_params& params = m_params.at(m_paramsList->currentIndex());

    QFileDialog dlg(this);
    dlg.setDirectory(QString::fromStdString(params.save_path));
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setOption(QFileDialog::ShowDirsOnly);

    if (dlg.exec())
    {
        QStringList files = dlg.selectedFiles();

        if (files.size() > 0)
        {
            QString nativePath = QDir::toNativeSeparators(files.at(0));
            m_torrentSavePath->setText(nativePath);
        }
    }
}

void AddTorrentDialog::onTorrentSavePathChanged(QString const& text)
{
    lt::add_torrent_params& params = m_params.at(m_paramsList->currentIndex());
    params.save_path = text.toStdString();
}

void AddTorrentDialog::onTorrentSequentialDownloadChanged(int state)
{
    lt::add_torrent_params& params = m_params.at(m_paramsList->currentIndex());

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
    lt::add_torrent_params& params = m_params.at(m_paramsList->currentIndex());

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
