#pragma once

#include "detailstab.hpp"

namespace pt
{
    namespace Ui
    {
        class TorrentOverviewWidget;
    }

    class TorrentOverviewWidget : public DetailsTab
    {
    public:
        TorrentOverviewWidget();
        ~TorrentOverviewWidget();

        virtual void clear() override;
        virtual void refresh(QList<TorrentHandle*> const& torrents) override;

    private:
        Ui::TorrentOverviewWidget* m_ui;
    };
}
