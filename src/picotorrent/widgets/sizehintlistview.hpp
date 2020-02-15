#pragma once

#include <QListView>

class SizeHintListView : public QListView
{
public:
    using QListView::QListView;

    QSize sizeHint() const override
    {
        return QSize(
            sizeHintForColumn(0),
            QListView::sizeHint().height());
    }
};
