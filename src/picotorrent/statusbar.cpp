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
        m_dhtNodeCount->setText(
            QString::asprintf(
                i18n("dht_i64d_nodes").toLocal8Bit().data(),
                nodes));
    }
}

void StatusBar::updateTorrentCount(int64_t torrents)
{
    m_torrentsCount->setText(
        QString::asprintf(
            i18n("i64d_torrents").toLocal8Bit().data(),
            torrents));
}

void StatusBar::updateTransferRates(int64_t downSpeed, int64_t upSpeed)
{
    m_transferSpeeds->setText(
        QString::asprintf(
            i18n("dl_s_ul_s").toLocal8Bit().data(),
            downSpeed < 1024 ? "-" : Utils::toStdString(Utils::toHumanFileSize(downSpeed)).c_str(),
            upSpeed   < 1024 ? "-" : Utils::toStdString(Utils::toHumanFileSize(upSpeed)).c_str()));
}
