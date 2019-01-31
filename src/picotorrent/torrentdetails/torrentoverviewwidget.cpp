#include "torrentoverviewwidget.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include "../elidedlabel.hpp"
#include "../sessionstate.hpp"
#include "../torrenthandle.hpp"
#include "../torrentstatus.hpp"
#include "../translator.hpp"

class BoldLabel : public QLabel
{
public:
    BoldLabel(QString const& text)
        : QLabel(text)
    {
        this->setStyleSheet("font-weight: bold;");
    }
};

namespace lt = libtorrent;
using pt::TorrentOverviewWidget;

TorrentOverviewWidget::TorrentOverviewWidget()
{
    m_name     = new ElidedLabel();
    m_infoHash = new ElidedLabel();
    m_savePath = new ElidedLabel();
    m_pieces   = new ElidedLabel();

    auto grid = new QGridLayout();
    grid->addWidget(new BoldLabel(i18n("name")),      0, 0);
    grid->addWidget(m_name,                           0, 1);
    grid->addWidget(new BoldLabel(i18n("info_hash")), 0, 2);
    grid->addWidget(m_infoHash,                       0, 3);
    grid->addWidget(new BoldLabel(i18n("save_path")), 1, 0);
    grid->addWidget(m_savePath,                       1, 1);
    grid->addWidget(new BoldLabel(i18n("pieces")),    1, 2);
    grid->addWidget(m_pieces,                         1, 3);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);

    auto layout = new QVBoxLayout();
    layout->addLayout(grid);
    layout->addStretch();

    this->clear();
    this->setLayout(layout);
}

void TorrentOverviewWidget::clear()
{
    m_name->setText("-");
    m_infoHash->setText("-");
    m_savePath->setText("-");
    m_pieces->setText("-");
}

void TorrentOverviewWidget::refresh(QList<pt::TorrentHandle*> const& torrents)
{
    if (torrents.count() != 1)
    {
        return;
    }

    TorrentHandle* torrent = torrents.at(0);
    TorrentStatus  status   = torrent->status();

    QString pieces = QString::asprintf(
        i18n("d_of_d").toLocal8Bit().data(),
        status.pieces.count(),
        status.pieces.size());

    m_name->setText(status.name);
    m_savePath->setText(status.savePath);
    m_infoHash->setText(status.infoHash);
    m_pieces->setText(pieces);
}
