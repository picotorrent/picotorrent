#include "statusbar.hpp"

#include <QLabel>

#include "core/utils.hpp"
#include "translator.hpp"

using pt::StatusBar;

StatusBar::StatusBar(QWidget* parent)
    : QStatusBar(parent)
{
    m_torrentsCount = new QLabel();
    m_torrentsCount->setMinimumWidth(100);
    m_dhtNodeCount = new QLabel();
    m_dhtNodeCount->setMinimumWidth(100);
    m_transferSpeeds = new QLabel();
    m_transferSpeeds->setMinimumWidth(100);

    this->addWidget(m_torrentsCount);
    this->addWidget(m_dhtNodeCount);
    this->addWidget(m_transferSpeeds);
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
    m_torrentsCount->setText(i18n("i64d_torrents").arg(torrents));
}

void StatusBar::updateTransferRates(int64_t downSpeed, int64_t upSpeed)
{
    m_transferSpeeds->setText(
        i18n("dl_s_ul_s").arg(
            downSpeed < 1024 ? "-" : QString::fromStdWString(Utils::toHumanFileSize(downSpeed)),
            upSpeed   < 1024 ? "-" : QString::fromStdWString(Utils::toHumanFileSize(upSpeed))));
}
