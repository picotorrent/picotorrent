#include "statusbar.hpp"

#include <QLabel>

#include "core/utils.hpp"
#include "translator.hpp"

using pt::StatusBar;

StatusBar::StatusBar(QWidget* parent)
    : QStatusBar(parent)
{
    m_torrentsCount = new QLabel(this);
    m_dhtNodeCount = new QLabel(this);
    m_transferSpeeds = new QLabel(this);

    this->addWidget(m_torrentsCount, 1);
    this->addWidget(m_dhtNodeCount, 1);
    this->addWidget(m_transferSpeeds, 1);
}

void StatusBar::updateDhtNodesCount(int64_t nodes)
{
    if (nodes < 0)
    {
        m_dhtNodeCount->setText(i18n("dht_disabled"));
    }
    else
    {
        m_dhtNodeCount->setText(i18n("dht_i64d_nodes").arg(nodes));
    }
}

void StatusBar::updateTorrentCount(int64_t torrents)
{
    m_torrentsCount->setText(
        i18n("i64d_torrents")
            .arg(torrents));
}

void StatusBar::updateTransferRates(int64_t downSpeed, int64_t upSpeed)
{
    m_transferSpeeds->setText(
        i18n("dl_s_ul_s")
            .arg(downSpeed < 1024 ? "-" : QString::fromStdWString(Utils::toHumanFileSize(downSpeed)))
            .arg(upSpeed   < 1024 ? "-" : QString::fromStdWString(Utils::toHumanFileSize(upSpeed))));
}
