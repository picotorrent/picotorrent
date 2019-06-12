#pragma once

#include <QFrame>

class QWidget;

namespace pt
{
    class SunkenLine : public QFrame
    {
    public:
        SunkenLine(QWidget* parent = nullptr)
            : QFrame(parent)
        {
            this->setFrameShape(QFrame::HLine);
            this->setFrameShadow(QFrame::Sunken);
        }
    };
}
