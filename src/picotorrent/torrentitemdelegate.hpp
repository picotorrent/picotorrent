#pragma once

#include <QStyledItemDelegate>

class QFont;
class QFontMetrics;
class QObject;

namespace pt
{
    class TorrentItemDelegate : public QStyledItemDelegate
    {
    public:
        TorrentItemDelegate(QObject* parent);
        virtual ~TorrentItemDelegate();

        virtual void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const;
        void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const override;

    private:
        QFont* m_font;
        QFontMetrics* m_fontMetrics;
    };
}
