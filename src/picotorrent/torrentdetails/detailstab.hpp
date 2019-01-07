#pragma once

#include <QWidget>

namespace pt
{
    struct DetailsTab : public QWidget
    {
        virtual void clear() = 0;
        virtual void refresh() = 0;
    };
}
