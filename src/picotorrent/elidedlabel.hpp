#pragma once

#include <QFrame>
#include <QString>

class QPaintEvent;
class QWidget;

namespace pt
{
    class ElidedLabel : public QFrame
    {
        Q_OBJECT

    public:
        explicit ElidedLabel(QWidget* parent = nullptr);
        explicit ElidedLabel(QString const& text, QWidget* parent = nullptr);

        void setText(const QString &text);
        const QString & text() const { return content; }
        bool isElided() const { return elided; }

    protected:
        void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    signals:
        void elisionChanged(bool elided);

    private:
        bool elided;
        QString content;
    };
}
