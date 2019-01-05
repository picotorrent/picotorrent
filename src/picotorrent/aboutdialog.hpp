#pragma once

#include <QDialog>

class QWidget;

namespace pt
{
    class AboutDialog : public QDialog
    {
    public:
        AboutDialog(QWidget* parent, std::string version);

    private:
        std::string m_version;
    };
}
