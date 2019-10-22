#pragma once

#include <QStatusBar>

class QLabel;
class QWidget;

namespace pt
{
    class StatusBar : public QStatusBar
    {
    public:
        StatusBar(QWidget* parent);

        void updateDhtNodesCount(int64_t nodes);
        void updateTorrentCount(int64_t torrents);
        void updateTransferRates(int64_t downSpeed, int64_t upSpeed);

    private:
        QLabel* m_torrentsCount;
        QLabel* m_dhtNodeCount;
        QLabel* m_transferSpeeds;
    };
}
