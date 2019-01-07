#pragma once

#include <QDialog>

class QWidget;

namespace pt
{
    class AboutDialog : public QDialog
    {
    public:
        AboutDialog(QWidget* parent);
    };
}
