#include "torrentoverviewwidget.hpp"
#include "ui_torrentoverviewwidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include "../torrenthandle.hpp"
#include "../torrentstatus.hpp"
#include "../translator.hpp"

using pt::TorrentOverviewWidget;

TorrentOverviewWidget::TorrentOverviewWidget()
    : m_ui(new Ui::TorrentOverviewWidget())
{
    m_ui->setupUi(this);
    m_ui->infoHashLabel->setText(i18n("info_hash"));
    m_ui->nameLabel->setText(i18n("name"));
    m_ui->piecesLabel->setText(i18n("pieces"));
    m_ui->savePathLabel->setText(i18n("save_path"));
}

TorrentOverviewWidget::~TorrentOverviewWidget()
{
    delete m_ui;
}

void TorrentOverviewWidget::clear()
{
    m_ui->name->setText("-");
    m_ui->infoHash->setText("-");
    m_ui->savePath->setText("-");
    m_ui->pieces->setText("-");
}

void TorrentOverviewWidget::refresh(QList<pt::TorrentHandle*> const& torrents)
{
    if (torrents.count() != 1)
    {
        return;
    }

    TorrentHandle* torrent = torrents.at(0);
    TorrentStatus  status  = torrent->status();

    QString pieces = i18n("d_of_d")
        .arg(status.pieces.count())
        .arg(status.pieces.size());

    m_ui->name->setText(status.name);
    m_ui->savePath->setText(status.savePath);
    m_ui->infoHash->setText(status.infoHash);
    m_ui->pieces->setText(pieces);
}
