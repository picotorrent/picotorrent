#pragma once

#include <QItemDelegate>

namespace pt
{
    class TorrentItemDelegate : public QItemDelegate
    {
    public:
        TorrentItemDelegate();
        void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const override;
    };
}
