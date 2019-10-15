#include "torrentitemdelegate.hpp"

#include <QApplication>
#include <QPainter>

#include "torrentlistmodel.hpp"

using pt::TorrentItemDelegate;

TorrentItemDelegate::TorrentItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
    m_font = new QFont(QApplication::font());
    m_font->setPointSize(8);
    m_fontMetrics = new QFontMetrics(*m_font);
}

TorrentItemDelegate::~TorrentItemDelegate()
{
    delete m_fontMetrics;
    delete m_font;
}

void TorrentItemDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    switch (index.column())
    {
    case TorrentListModel::Columns::Progress:
    {
        // Set up a QStyleOptionProgressBar to precisely mimic the
        // environment of a progress bar.
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
        int progress = index.data().toDouble() * 100;
        progressBarOption.progress = progress < 0 ? 0 : progress;
        progressBarOption.text = QString("%1%").arg(progressBarOption.progress);

        // Draw the progress bar onto the view.
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);

        break;
    }
    }
}
