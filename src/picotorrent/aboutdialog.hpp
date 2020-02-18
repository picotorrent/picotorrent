#pragma once

#include <QDialog>

class QWidget;

namespace Ui
{
    class AboutDialog;
}

namespace pt
{
    class AboutDialog : public QDialog
    {
    public:
        AboutDialog(QWidget* parent);
        ~AboutDialog();

    private:
        ::Ui::AboutDialog* m_ui;
    };
}
