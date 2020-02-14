#include "addtorrentdialog.hpp"

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
#include "core/utils.hpp"

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

    m_paramsList = new QComboBox(this);
    m_torrentName = new QLabel("-", this);
    m_torrentSize = new QLabel("-", this);
    m_torrentInfoHash = new QLabel("-", this);
    m_torrentComment = new QLabel("-", this);
    m_torrentSavePath = new QLineEdit(this);
    m_torrentSavePathBrowse = new QPushButton(i18n("browse"), this);
    m_torrentSavePathBrowse->setMaximumWidth(30);
    m_torrentSequentialDownload = new QCheckBox(i18n("sequential_download"), this);
    m_torrentStart = new QCheckBox(i18n("start_torrent"), this);
    m_filesModel = new FileStorageItemModel();
    m_torrentContextMenu = new QMenu(this);
    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    m_torrentFiles = new QTreeView(this);
    m_torrentFiles->setContextMenuPolicy(Qt::CustomContextMenu);
    m_torrentFiles->setModel(m_filesModel);
    m_torrentFiles->hideColumn(FileStorageItemModel::Columns::Progress);
    m_torrentFiles->setColumnWidth(FileStorageItemModel::Columns::Name, 240);
    m_torrentFiles->setColumnWidth(FileStorageItemModel::Columns::Size, 60);
    m_torrentFiles->setSelectionMode(QTreeView::ExtendedSelection);

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

    auto fileGroup = new QGroupBox(i18n("file"), this);
    auto fileLayout = new QVBoxLayout();
    fileLayout->addWidget(m_paramsList);
    fileGroup->setLayout(fileLayout);

    auto torrentGroup = new QGroupBox(i18n("torrent"), this);
    auto torrentGrid = new QGridLayout();
    torrentGrid->addWidget(new QLabel(i18n("name"), this), 0, 0);
    torrentGrid->addWidget(m_torrentName, 0, 1);
    torrentGrid->addWidget(new QLabel(i18n("size"), this), 1, 0);
    torrentGrid->addWidget(m_torrentSize, 1, 1);
    torrentGrid->addWidget(new QLabel(i18n("info_hash"), this), 2, 0);
    torrentGrid->addWidget(m_torrentInfoHash, 2, 1);
    torrentGrid->addWidget(new QLabel(i18n("comment"), this), 3, 0);
    torrentGrid->addWidget(m_torrentComment, 3, 1);
    torrentGrid->setColumnStretch(0, 1);
    torrentGrid->setColumnStretch(1, 2);
    torrentGroup->setLayout(torrentGrid);

    auto prefsGroup = new QGroupBox(i18n("storage"), this);
    auto prefsGrid = new QGridLayout();
    prefsGrid->addWidget(new QLabel(i18n("save_path"), this), 0, 0);

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

    auto layout = new QVBoxLayout();
    layout->addWidget(fileGroup);
    layout->addWidget(torrentGroup);
    layout->addWidget(prefsGroup);
    layout->addWidget(m_buttons);
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
        m_buttons,
        &QDialogButtonBox::accepted,
        this, &QDialog::accept);

    connect(
        m_buttons,
        &QDialogButtonBox::rejected,
        this, &QDialog::reject);

    this->setLayout(layout);
    this->setMinimumWidth(430);
    this->setWindowFlags(flags);
    this->setWindowTitle(i18n("add_torrent_s"));

    // Add torrents

    for (lt::add_torrent_params const& p : m_params)
    {
        m_paramsList->addItem(getDisplayName(p));
    }
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
            m_paramsList->setItemText(i, QString::fromStdString(m_params.at(i).ti->name()));

            if (i == m_paramsList->currentIndex())
            {
                onTorrentIndexChanged(i);
            }
        }
    }
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

    m_torrentName->setText(getDisplayName(param));
    m_torrentSize->setText(getDisplaySize(param));
    m_torrentInfoHash->setText(getDisplayHash(param));
    m_torrentComment->setText(getDisplayComment(param));
    m_torrentSavePath->setText(QString::fromStdString(param.save_path));

    m_torrentSequentialDownload->setChecked(
        ((param.flags & lt::torrent_flags::sequential_download) == lt::torrent_flags::sequential_download));

    m_torrentStart->setChecked(
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
                             m_torrentSavePath->setText(nativePath);
                         }
                     });

    QObject::connect(dlg, &QFileDialog::finished,
                     dlg, &QFileDialog::deleteLater);
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
