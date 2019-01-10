#pragma once

#include <QItemDelegate>

class QFont;
class QFontMetrics;

namespace pt
{
    class FileStorageItemDelegate : public QItemDelegate
    {
    public:
        FileStorageItemDelegate();
        virtual ~FileStorageItemDelegate();

        void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const override;

    private:
        QFont* m_font;
        QFontMetrics* m_fontMetrics;
    };
}
