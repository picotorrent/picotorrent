#include "elidedlabel.hpp"

#include <QPainter>
#include <QTextLayout>
#include <QWidget>

using pt::ElidedLabel;

ElidedLabel::ElidedLabel(const QString &text, QWidget *parent)
    : QFrame(parent),
    elided(false),
    content(text)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

ElidedLabel::ElidedLabel(QWidget *parent)
    : ElidedLabel("", parent)
{
}

void ElidedLabel::setText(const QString &newText)
{
    content = newText;
    update();
}

void ElidedLabel::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    QFontMetrics fontMetrics = painter.fontMetrics();

    bool didElide = false;
    int lineSpacing = fontMetrics.lineSpacing();
    int y = 0;

    QTextLayout textLayout(content, painter.font());
    textLayout.beginLayout();
    forever
    {
        QTextLine line = textLayout.createLine();

        if (!line.isValid())
        {
            break;
        }

        line.setLineWidth(width());
        int nextLineY = y + lineSpacing;

        if (height() >= nextLineY + lineSpacing)
        {
            line.draw(&painter, QPoint(0, y));
            y = nextLineY;
            //! [2]
            //! [3]
        }
        else
        {
            QString lastLine = content.mid(line.textStart());
            QString elidedLastLine = fontMetrics.elidedText(lastLine, Qt::ElideRight, width());
            painter.drawText(QPoint(0, y + fontMetrics.ascent()), elidedLastLine);
            line = textLayout.createLine();
            didElide = line.isValid();
            break;
        }
    }

    textLayout.endLayout();

    if (didElide != elided)
    {
        elided = didElide;
        emit elisionChanged(didElide);
    }
}
