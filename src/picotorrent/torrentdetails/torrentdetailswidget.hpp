#pragma once

#include <QList>
#include <QTabWidget>

#include <memory>
#include <vector>

class QWidget;

namespace pt
{
    class GeoIP;
    class TorrentHandle;

    namespace Ui
    {
        class TorrentDetailsWidget;
    }

    class TorrentDetailsWidget : public QTabWidget
    {
    public:
        TorrentDetailsWidget(QWidget* parent);
        void setGeo(GeoIP* geo);

    public slots:
        void update(QList<TorrentHandle*> const& torrents);

    private:
        Ui::TorrentDetailsWidget* m_ui;
    };
}
