#include "torrentdetailswidget.hpp"

using pt::TorrentDetailsWidget;

TorrentDetailsWidget::TorrentDetailsWidget()
{
    this->setMinimumHeight(100);
    this->setMovable(true);
}

void TorrentDetailsWidget::addTab(pt::Tab const& tab)
{
    // TODO(validation)

    // TODO(sorting)
    // Insert the tab in our tabs vector
    // Sort on storedPosition first, making the explicitly positioned tabs show up first
    // Then sort on the provided sortOrder

    m_tabs.push_back(tab);

    std::sort(
        m_tabs.begin(),
        m_tabs.end(),
        [](Tab const& t1, Tab const& t2)
        {
            return t1.sortOrder < t2.sortOrder;
        });

    QTabWidget::setUpdatesEnabled(false);
    QTabWidget::clear();

    for (Tab const& tab : m_tabs)
    {
        QTabWidget::addTab(tab.widget, tab.title);
    }

    QTabWidget::setUpdatesEnabled(true);
}
