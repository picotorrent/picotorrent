#include "filestorageitemdelegate.hpp"

#include <QApplication>
#include <QPainter>

#include "filestorageitemmodel.hpp"

using pt::FileStorageItemDelegate;

FileStorageItemDelegate::FileStorageItemDelegate()
{
    m_font = new QFont(QApplication::font());
    m_font->setPointSize(8);
    m_fontMetrics = new QFontMetrics(*m_font);
}

FileStorageItemDelegate::~FileStorageItemDelegate()
{
    delete m_fontMetrics;
    delete m_font;
}

void FileStorageItemDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    switch (index.column())
    {
    case FileStorageItemModel::Columns::Progress:
    {
        int progress = index.data().toDouble() * 100;

        if (progress < 0)
        {
            break;
        }

        painter->setFont(*m_font);

        QStyleOptionProgressBar progressBarOption;
        progressBarOption.state = QStyle::State_Enabled;
        progressBarOption.direction = QApplication::layoutDirection();
        progressBarOption.rect = option.rect - QMargins(1, 1, 1, 1);
        progressBarOption.fontMetrics = *m_fontMetrics;
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.textAlignment = Qt::AlignCenter;
        progressBarOption.textVisible = true;

        // Set the progress and text values of the style option.
        progressBarOption.progress = progress < 0 ? 0 : progress;
        progressBarOption.text = QString::asprintf("%d%%", progressBarOption.progress);

        // Draw the progress bar onto the view.
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);

        break;
    }
    default:
        return QItemDelegate::paint(painter, option, index);
    }
}
