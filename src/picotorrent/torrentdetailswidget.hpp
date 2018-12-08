#pragma once

#include <QTabWidget>

#include <picotorrent.hpp>
#include <vector>

namespace pt
{
    class TorrentDetailsWidget : public QTabWidget, public ITorrentDetailsWidget
    {
    public:
        TorrentDetailsWidget();
        void addTab(Tab const& tab) override;

    private:
        std::vector<Tab> m_tabs;
    };
}
