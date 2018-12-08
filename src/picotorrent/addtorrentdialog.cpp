#include "addtorrentdialog.hpp"

#include <filesystem>
#include <sstream>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
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
#include "utils.hpp"

namespace fs = std::experimental::filesystem;
namespace lt = libtorrent;
using pt::AddTorrentDialog;

AddTorrentDialog::AddTorrentDialog(QWidget* parent, std::vector<lt::add_torrent_params> const& params)
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
    m_filesModel = new FileStorageItemModel();
    m_torrentContextMenu = new QMenu();

    m_torrentFiles = new QTreeView();
    m_torrentFiles->setContextMenuPolicy(Qt::CustomContextMenu);
    m_torrentFiles->setModel(m_filesModel);
    m_torrentFiles->hideColumn(FileStorageItemModel::Columns::Progress);
    m_torrentFiles->setColumnWidth(FileStorageItemModel::Columns::Name, 240);
    m_torrentFiles->setColumnWidth(FileStorageItemModel::Columns::Size, 60);

    // Menu
    QMenu* priorities = m_torrentContextMenu->addMenu("Priority");
    priorities->addAction("Maximum")
        ->setData(FilePriority::Max);
    priorities->addAction("Normal")
        ->setData(FilePriority::Normal);
    priorities->addAction("Low")
        ->setData(FilePriority::Low);
    priorities->addSeparator();
    priorities->addAction("Do not download")
        ->setData(FilePriority::Skip);

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
    dirBrowseLayout->addWidget(new QLineEdit());
    dirBrowseLayout->addWidget(new QPushButton("Browse"));
    dirBrowseLayout->setStretch(0, 1);
    prefsGrid->addLayout(dirBrowseLayout, 0, 1);

    auto optsLayout = new QHBoxLayout();
    optsLayout->addWidget(new QCheckBox("Sequential download"));
    optsLayout->addWidget(new QCheckBox("Start torrent"));
    prefsGrid->addLayout(optsLayout, 1, 1);

    prefsGrid->addWidget(m_torrentFiles, 2, 0, -1, -1);
    prefsGrid->setRowStretch(2, 1);

    prefsGroup->setLayout(prefsGrid);

    auto btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    btnLayout->addWidget(new QPushButton("OK"));
    btnLayout->addWidget(new QPushButton("Cancel"));

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
        m_torrentFiles,
        &QTreeView::customContextMenuRequested,
        this,
        &AddTorrentDialog::onTorrentFileContextMenu);

    connect(
        m_torrentContextMenu,
        &QMenu::triggered,
        this,
        &AddTorrentDialog::onSetTorrentFilePriorities);

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

void AddTorrentDialog::onSetTorrentFilePriorities(QAction* action)
{
    FilePriority prio = static_cast<FilePriority>(action->data().toInt());
    auto const& indices = m_torrentFiles->selectionModel()->selectedIndexes();
    auto fileIndices = m_filesModel->fileIndices(indices);
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
    m_torrentSize->setText(Utils::ToHumanFileSize(param.ti->total_size()));
    m_torrentInfoHash->setText(QString::fromStdString(hash.str()));
    m_torrentComment->setText(QString::fromStdString(param.ti->comment()));

    m_filesModel->rebuildTree(param.ti);

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
