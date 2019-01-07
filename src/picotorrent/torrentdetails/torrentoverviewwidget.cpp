#include "torrentoverviewwidget.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include "../elidedlabel.hpp"
#include "../sessionstate.hpp"
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

TorrentOverviewWidget::TorrentOverviewWidget(std::shared_ptr<pt::SessionState> state)
    : m_state(state)
{
    m_name = new ElidedLabel();
    m_infoHash = new ElidedLabel();
    m_savePath = new ElidedLabel();
    m_pieces = new ElidedLabel();

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

void TorrentOverviewWidget::refresh()
{
    if (m_state->selectedTorrents.size() != 1)
    {
        return;
    }

    auto hash = (*m_state->selectedTorrents.begin());
    auto th = m_state->torrents.at(hash);
    auto ts = th.status();

    std::stringstream ss;
    ss << hash;

    QString pieces;
    pieces.sprintf(
        i18n("d_of_d").toLocal8Bit().data(),
        ts.pieces.count(),
        ts.pieces.size());

    m_name->setText(QString::fromStdString(ts.name));
    m_savePath->setText(QString::fromStdString(ts.save_path));
    m_infoHash->setText(QString::fromStdString(ss.str()));
    m_pieces->setText(pieces);
}
